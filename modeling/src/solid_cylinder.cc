#include "modeling/solid_cylinder.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include "base/float.h"
#include "base/float_cmp.h"
#include "math/consts/pi.h"
#include "math/linalg/ray.h"
#include "math/linalg/vector3f.h"
#include "math/sqrt.h"
#include "math/utils.h"
#include "ray_tracing/cylinder_z_infinite.h"

// TODO(a.kerimov): Try constexpr.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define SOLID_CYLINDER_DEBUG_LEVEL 2

#if SOLID_CYLINDER_DEBUG_LEVEL >= 1
#include <iostream>
#include "math/linalg/vector_io.h"
#else
#include <iostream>  // TODO(a.kerimov): Remove.
#include "base/ignore_unused.h"
#endif

namespace {

// TODO(a.kerimov): Написать тесты.

class SolidCylinderWorker {
 public:
  explicit constexpr SolidCylinderWorker(const SolidCylinder& c) noexcept
      : c_{c} {}

  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  WorkerResult SolveDir(const WorkerParams& params) {
    WorkerResult result;
    result.absorbed.resize(c_.cylinders.size());

    auto intensity = params.intensity;

    // TODO(a.kerimov): Move to params if needed.
    assert(c_.cylinders.size() > 1);
    const auto border_idx = c_.cylinders.size() - 1;
    current_cylinder_idx_ = border_idx;

    ray_ = params.ray;
    // TODO(a.kerimov): Revive geogebra output (see git history).

    use_prev_ = params.use_prev;

    for ([[maybe_unused]] size_t i = 0; intensity > params.intensity_end; ++i) {
#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
      std::cout << "[SolidCylinder iteration=" << i
                << ", intensity=" << intensity << ", use_prev=" << use_prev_
                << "]\n";
#endif

      Intersect();

      const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
      const auto dr = Vector3F::Distance(prev_pos_, ray_.pos);
      const auto k = c_.attenuations[idx];
      const auto exp = std::exp(-k * dr);
      const auto prev_intensity = intensity;
      intensity *= exp;
      result.absorbed[idx] += prev_intensity - intensity;

#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
      std::cout << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx_
                << "][ci=" << current_cylinder_idx_ << "][dir=" << ray_.dir
                << "]\n";
#endif

      if (current_cylinder_idx_ == border_idx) {
        const auto& p = c_.params();
        constexpr auto kOutward = true;
        const auto res = c_.cylinders[border_idx].Refract(
            ray_, p.refractive_index, p.refractive_index_external, p.mirror,
            kOutward);

        if (res.T > 0) {
          if (const auto IT = intensity * res.T; IT > params.intensity_end) {
            result.released_rays.push_back({{ray_.pos, res.refracted},
                                            IT,
                                            params.intensity_end,
                                            !kOutward});
          } else {
            result.absorbed_at_the_border += IT;
          }
        }

        assert(res.R > 0);
        ray_.dir = res.reflected;
        intensity *= res.R;
        use_prev_ = !use_prev_;
#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
        std::cout << "REFLECT, new dir " << ray_.dir << '\n';
#endif
      }
    }

#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
    std::cout << "[SolidCylinder iteration=LAST, intensity=" << intensity
              << "]\n";
#endif

    Intersect();

    const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
    result.absorbed[idx] += intensity;

    return result;
  }

  Float CalculateIntensity(const Vector3F initial_pos,
                           const Vector3F dir,
                           const std::size_t sphere_points) {
    const auto ndir = -dir;

    ray_ = {initial_pos, ndir};
    assert(c_.cylinders.size() > 1);
    const auto border_idx = c_.cylinders.size() - 1;
    const auto t = c_.cylinders[border_idx].IntersectCurr(ray_);
    assert(t > 0);

    ray_.pos = ray_.Point(t);
    ray_.dir = dir;

    current_cylinder_idx_ = border_idx;
    Float intensity{};
    use_prev_ = true;

    for ([[maybe_unused]] size_t i = 0;; ++i) {
#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
      std::cout << "[SolidCylinder::CalculateIntensity, iteration=" << i
                << ", intensity=" << intensity << "]\n";
#endif

      Intersect();

      const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
      const auto dr = Vector3F::Distance(prev_pos_, ray_.pos);
      const auto k = c_.attenuations[idx];
      const auto exp = std::exp(-k * dr);
      intensity *= exp;
      intensity += c_.intensities[idx] * (1 - exp);

#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
      std::cout << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx_
                << "][ci=" << current_cylinder_idx_ << "][dir=" << ray_.dir
                << "]\n";
#endif

      if (current_cylinder_idx_ == border_idx) {
        break;
      }
    }

    // const auto cos_theta = dir.z();
    // const auto sin_phi = Sqrt(Sqr(dir.x()) + Sqr(dir.y()));

    std::cout << "INTENSITY BEFORE COSINE: " << intensity << "\n";

    intensity *= 2 * 2 * consts::pi / static_cast<int>(sphere_points) * dir.x();

    return intensity;
  }

 private:
  void PrintT(const char* prompt, const Float t) const {
#if SOLID_CYLINDER_DEBUG_LEVEL >= 2
    std::cout << prompt << t;
    if (t > kEps) {
      std::cout << ' ' << ray_.Point(t);
    }
    std::cout << '\n';
#else
    IgnoreUnused(prompt);
    IgnoreUnused(t);
#endif
  }

  void IntersectPrevCylinder() {
    if (current_cylinder_idx_ > 0) {
      const auto t = c_.cylinders[current_cylinder_idx_ - 1].Intersect(ray_);
      ts_[kIdxPrevCylinder] = t;

      PrintT("PrevCylinder  ", t);
    } else {
      ts_[kIdxPrevCylinder] = -1;
    }
  }

  void IntersectNextCylinder() {
    if (current_cylinder_idx_ + 1 < c_.cylinders.size()) {
      const auto t = c_.cylinders[current_cylinder_idx_ + 1].Intersect(ray_);
      ts_[kIdxNextCylinder] = t;

      PrintT("NextCylinder  ", t);
    } else {
      ts_[kIdxNextCylinder] = -1;
    }
  }

  void IntersectCurrCylinder() {
    const auto t = c_.cylinders[current_cylinder_idx_].IntersectCurr(ray_);
    ts_[kIdxCurrCylinder] = IsZero(t, kEps) || !use_prev_ ? -1 : t;

    PrintT("CurrCylinder* ", t);
  }

  void Intersect() {
    IntersectPrevCylinder();
    IntersectNextCylinder();
    IntersectCurrCylinder();

    t_min_idx_ = FindMinimalNonNegativeIndex(ts_);
    assert(t_min_idx_ <= kIdxLastCylinder);
    prev_pos_ = ray_.pos;
    ray_.pos = ray_.Point(ts_[t_min_idx_]);

    prev_cylinder_idx_ = current_cylinder_idx_;

    if (t_min_idx_ == kIdxPrevCylinder) {
      --current_cylinder_idx_;
    } else if (t_min_idx_ == kIdxNextCylinder) {
      ++current_cylinder_idx_;
    } else {
      use_prev_ = !use_prev_;
    }
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const SolidCylinder& c_;

  Ray ray_{};
  Vector3F prev_pos_;

  static constexpr size_t kIdxPrevCylinder = 0;
  static constexpr size_t kIdxNextCylinder = 1;
  static constexpr size_t kIdxCurrCylinder = 2;
  static constexpr size_t kIdxLastCylinder = kIdxCurrCylinder;
  std::array<Float, 3> ts_{};
  std::size_t t_min_idx_{};

  std::size_t current_cylinder_idx_{};
  std::size_t prev_cylinder_idx_{};
  bool use_prev_{false};
};

}  // namespace

SolidCylinder::SolidCylinder(const Params& params,
                             const TemperatureFunc& temperature,
                             const IntensityFunc& intensity,
                             const AttenuationFunc& attenuation)
    : params_{params} {
  assert(params_.steps > 1);
  cylinders.reserve(params_.steps);
  temperatures.reserve(params_.steps);
  intensities.reserve(params_.steps);
  attenuations.reserve(params_.steps);

  const auto step = params_.radius / static_cast<Float>(params_.steps);

  const auto insert_cylinder = [&temperature, &intensity, &attenuation, this,
                                step](const Float radius) {
    cylinders.emplace_back(params_.center, radius);
    const auto T = temperature((radius - step / 2) / params_.radius);
    temperatures.emplace_back(T);
    intensities.emplace_back(intensity(T));
    attenuations.emplace_back(attenuation(T));
  };

  for (std::size_t i = 1; i < params_.steps; ++i) {
    insert_cylinder(step * static_cast<Float>(i));
  }

  insert_cylinder(params_.radius);

  assert(cylinders.size() == params_.steps);
  assert(temperatures.size() == params_.steps);
  assert(intensities.size() == params_.steps);
  assert(attenuations.size() == params_.steps);

#if SOLID_CYLINDER_DEBUG_LEVEL >= 1
  std::cout << "[SolidCylinder]\n"
               "Cylinders:\n";
  for (auto& cylinder : cylinders) {
    std::cout << cylinder.center() << ' ' << Sqrt(cylinder.radius2()) << '\n';
  }

  std::cout << "T:\n";
  for (const auto t : temperatures) {
    std::cout << t << '\n';
  }

  std::cout << "I:\n";
  for (const auto i : intensities) {
    std::cout << i << '\n';
  }
#endif
}

WorkerResult SolidCylinder::SolveDir(const WorkerParams& params) const {
  SolidCylinderWorker worker{*this};
  return worker.SolveDir(params);
}

Float SolidCylinder::CalculateIntensity(const Vector3F initial_pos,
                                        const Vector3F dir,
                                        const std::size_t sphere_points) const {
  SolidCylinderWorker worker{*this};
  return worker.CalculateIntensity(initial_pos, dir, sphere_points);
}
