#include "modeling/hollow_cylinder.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include "base/float.h"
#include "base/float_cmp.h"
#include "base/ignore_unused.h"
#include "math/linalg/ray.h"
#include "math/linalg/vector3f.h"
#include "math/sqrt.h"
#include "math/utils.h"
#include "ray_tracing/cylinder_z_infinite.h"

// TODO(a.kerimov): Try constexpr.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define HOLLOW_CYLINDER_DEBUG_LEVEL 2

#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 1
#include <iostream>
#include "math/linalg/vector_io.h"
#else
#include "base/ignore_unused.h"
#endif

namespace {

// TODO(a.kerimov): Написать тесты.

class HollowCylinderWorker {
 public:
  explicit constexpr HollowCylinderWorker(const HollowCylinder& c) noexcept
      : c_{c} {}

  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  WorkerResult SolveDir(const WorkerParams& params) {
    WorkerResult result;
    result.absorbed.resize(c_.cylinders.size());

    auto intensity = params.intensity;

    // TODO(a.kerimov): Move to params if needed.
    assert(c_.cylinders.size() > 2);
    const auto border_idx = c_.cylinders.size() - 1;
    current_cylinder_idx_ = 0;

    ray_ = params.ray;
    // TODO(a.kerimov): Revive geogebra output (see git history).

    use_prev_ = params.use_prev;
    assert(!use_prev_);

    for ([[maybe_unused]] size_t i = 0; intensity > params.intensity_end; ++i) {
#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 2
      std::cout << "[HollowCylinder iteration=" << i
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
      assert(idx != 0);

#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 2
      std::cout << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx_
                << "][ci=" << current_cylinder_idx_ << "][dir=" << ray_.dir
                << "]\n";
#endif

      const auto outward = current_cylinder_idx_ == border_idx;
      if (outward || current_cylinder_idx_ == 0) {
        assert(outward == !use_prev_);
        const auto& p = c_.params();
        const auto eta_t =
            outward ? p.refractive_index_external : p.refractive_index_internal;
        const auto mirror = outward ? p.mirror_external : p.mirror_internal;
        const auto res = c_.cylinders[current_cylinder_idx_].Refract(
            ray_, p.refractive_index, eta_t, mirror, outward);

        if (res.T > 0) {
          if (const auto IT = intensity * res.T; IT > params.intensity_end) {
            result.released_rays.push_back({{ray_.pos, res.refracted},
                                            IT,
                                            params.intensity_end,
                                            use_prev_});
          } else if (outward) {
            result.absorbed_at_the_border += IT;
          } else {
            result.absorbed[0] += IT;
          }
        }

        assert(res.R > 0);
        ray_.dir = res.reflected;
        intensity *= res.R;
        use_prev_ = outward;
#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 2
        std::cout << "REFLECT, new dir " << ray_.dir << '\n';
#endif
      }
    }

#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 2
    std::cout << "[HollowCylinder iteration=LAST, intensity=" << intensity
              << "]\n";
#endif

    Intersect();

    const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
    result.absorbed[idx] += intensity;
    assert(idx != 0);

    return result;
  }

 private:
  void PrintT(const char* prompt, const Float t) const {
#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 2
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
    IgnoreUnused(kIdxLastCylinder);
    prev_pos_ = ray_.pos;
    // TODO(a.kerimov): Fix all Point-s.
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
  const HollowCylinder& c_;

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

HollowCylinder::HollowCylinder(const Params& params,
                               const TemperatureFunc& temperature,
                               const IntensityFunc& intensity,
                               const AttenuationFunc& attenuation)
    : params_{params} {
  assert(params_.radius_max > params_.radius_min);
  assert(params_.steps > 1);
  cylinders.reserve(params_.steps + 1);
  temperatures.reserve(params_.steps + 1);
  intensities.reserve(params_.steps + 1);
  attenuations.reserve(params_.steps + 1);

  const auto step = (params_.radius_max - params_.radius_min) /
                    static_cast<Float>(params_.steps);

  const auto insert_cylinder = [&temperature, &intensity, &attenuation, this,
                                step](const Float radius) {
    cylinders.emplace_back(params_.center, radius);
    const auto T = temperature((radius - step / 2) / params_.radius_min);
    temperatures.emplace_back(T);
    intensities.emplace_back(intensity(T));
    attenuations.emplace_back(attenuation(T));
  };

  insert_cylinder(params_.radius_min);

  for (std::size_t i = 1; i < params_.steps; ++i) {
    insert_cylinder(params_.radius_min + step * static_cast<Float>(i));
  }

  insert_cylinder(params_.radius_max);

  assert(cylinders.size() == params_.steps + 1);
  assert(temperatures.size() == params_.steps + 1);
  assert(intensities.size() == params_.steps + 1);
  assert(attenuations.size() == params_.steps + 1);

#if HOLLOW_CYLINDER_DEBUG_LEVEL >= 1
  std::cout << "[HollowCylinder]\n"
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

WorkerResult HollowCylinder::SolveDir(const WorkerParams& params) const {
  HollowCylinderWorker worker{*this};
  return worker.SolveDir(params);
}
