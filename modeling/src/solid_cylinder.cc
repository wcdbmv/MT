#include "modeling/solid_cylinder.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "base/config/float.h"
#include "base/ignore_unused.h"
#include "math/consts/pi.h"
#include "math/float/compare.h"
#include "math/float/eps.h"
#include "math/linalg/vector.h"
#include "math/linalg/vector_io.h"
#include "ray_tracing/cylinder_z_infinite.h"
#include "ray_tracing/utils.h"

namespace {

// TODO(a.kerimov): Написать тесты.

constexpr int kDebugLevel = 0;  // 0-2.

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

    pos_ = params.pos;
    dir_ = params.dir;
    // TODO(a.kerimov): Revive geogebra output (see git history).

    use_prev_ = params.use_prev;

    for ([[maybe_unused]] size_t i = 0; intensity > params.intensity_end; ++i) {
      if constexpr (kDebugLevel >= 2) {
        std::cout << "[SolidCylinder iteration=" << i
                  << ", intensity=" << intensity << ", use_prev=" << use_prev_
                  << "]\n";
      }

      Intersect();

      const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
      const auto dr = Vec3::Distance(prev_pos_, pos_);
      const auto k = c_.attenuations[idx];
      const auto exp = std::exp(-k * dr);
      const auto prev_intensity = intensity;
      intensity *= exp;
      result.absorbed[idx] += prev_intensity - intensity;

      if constexpr (kDebugLevel >= 2) {
        std::cout << "NEW POS: " << pos_ << " [ti=" << t_min_idx_
                  << "][ci=" << current_cylinder_idx_ << "][dir=" << dir_
                  << "]\n";
      }

      if (current_cylinder_idx_ == border_idx) {
        const auto& p = c_.params();
        constexpr auto kOutward = true;
        const auto res = c_.cylinders[border_idx].Refract(
            pos_, dir_, p.refractive_index, p.refractive_index_external,
            p.mirror, kOutward);

        if (res.T > 0) {
          if (const auto new_i = intensity * res.T;
              new_i > params.intensity_end) {
            result.released_rays.push_back(
                {pos_, res.refracted, new_i, params.intensity_end, !kOutward});
          } else {
            result.absorbed_at_the_border += new_i;
          }
        }

        assert(res.R > 0);
        dir_ = res.reflected;
        intensity *= res.R;
        use_prev_ = !use_prev_;
        if constexpr (kDebugLevel >= 2) {
          std::cout << "REFLECT, new dir " << dir_ << '\n';
        }
      }
    }

    if constexpr (kDebugLevel >= 2) {
      std::cout << "[SolidCylinder iteration=LAST, intensity=" << intensity
                << "]\n";
    }

    Intersect();

    const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
    result.absorbed[idx] += intensity;

    return result;
  }

  Float CalculateIntensity(Vec3 initial_pos,
                           Vec3 dir,
                           std::size_t sphere_points) {
    const auto ndir = -dir;

    pos_ = initial_pos;
    dir_ = ndir;
    assert(c_.cylinders.size() > 1);
    const auto border_idx = c_.cylinders.size() - 1;
    const auto t = c_.cylinders[border_idx].IntersectCurr(pos_, dir_);
    assert(t > 0);

    pos_ = pos_ + t * dir_;
    dir_ = dir;

    current_cylinder_idx_ = border_idx;
    Float intensity{};
    use_prev_ = true;

    for ([[maybe_unused]] size_t i = 0;; ++i) {
      if constexpr (kDebugLevel >= 2) {
        std::cout << "[SolidCylinder::CalculateIntensity, iteration=" << i
                  << ", intensity=" << intensity << "]\n";
      }

      Intersect();

      const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
      const auto dr = Vec3::Distance(prev_pos_, pos_);
      const auto k = c_.attenuations[idx];
      const auto exp = std::exp(-k * dr);
      intensity *= exp;
      intensity += c_.intensities[idx] * (1 - exp);

      if constexpr (kDebugLevel >= 2) {
        std::cout << "NEW POS: " << pos_ << " [ti=" << t_min_idx_
                  << "][ci=" << current_cylinder_idx_ << "][dir=" << dir_
                  << "]\n";
      }

      if (current_cylinder_idx_ == border_idx) {
        break;
      }
    }

    // const auto cos_theta = dir.z();
    // const auto sin_phi = std::sqrt(Sqr(dir.x()) + Sqr(dir.y()));

    std::cout << "INTENSITY BEFORE COSINE: " << intensity << "\n";

    intensity *=
        2 * 2 * consts::kPi / static_cast<int>(sphere_points) * dir.x();

    return intensity;
  }

 private:
  void PrintT(const char* prompt, Float t) const {
    if constexpr (kDebugLevel >= 2) {
      std::cout << prompt << t;
      if (t > kEps) {
        std::cout << ' ' << pos_ + t * dir_;
      }
      std::cout << '\n';
    }
  }

  void IntersectPrevCylinder() {
    if (current_cylinder_idx_ > 0) {
      const auto t =
          c_.cylinders[current_cylinder_idx_ - 1].Intersect(pos_, dir_);
      ts_[kIdxPrevCylinder] = t;

      PrintT("PrevCylinder  ", t);
    } else {
      ts_[kIdxPrevCylinder] = -1;
    }
  }

  void IntersectNextCylinder() {
    if (current_cylinder_idx_ + 1 < c_.cylinders.size()) {
      const auto t =
          c_.cylinders[current_cylinder_idx_ + 1].Intersect(pos_, dir_);
      ts_[kIdxNextCylinder] = t;

      PrintT("NextCylinder  ", t);
    } else {
      ts_[kIdxNextCylinder] = -1;
    }
  }

  void IntersectCurrCylinder() {
    const auto t =
        c_.cylinders[current_cylinder_idx_].IntersectCurr(pos_, dir_);
    ts_[kIdxCurrCylinder] = IsZero(t, kEps) ? -1 : t;

    PrintT("CurrCylinder* ", t);
  }

  void Intersect() {
    IntersectPrevCylinder();
    IntersectNextCylinder();
    IntersectCurrCylinder();

    t_min_idx_ = FindIndexOfMinimalNonNegative(ts_);
    assert(t_min_idx_ <= kIdxLastCylinder);
    IgnoreUnused(kIdxLastCylinder);
    prev_pos_ = pos_;
    pos_ += ts_[t_min_idx_] * dir_;

    prev_cylinder_idx_ = current_cylinder_idx_;

    if (t_min_idx_ == kIdxPrevCylinder) {
      --current_cylinder_idx_;
    } else if (t_min_idx_ == kIdxNextCylinder) {
      ++current_cylinder_idx_;
    }

    use_prev_ = current_cylinder_idx_ <= prev_cylinder_idx_;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const SolidCylinder& c_;

  Vec3 pos_;
  Vec3 dir_;
  Vec3 prev_pos_;

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
    const auto t = temperature((radius - step / 2) / params_.radius);
    temperatures.emplace_back(t);
    intensities.emplace_back(intensity(t));
    attenuations.emplace_back(attenuation(t));
  };

  for (std::size_t i = 1; i < params_.steps; ++i) {
    insert_cylinder(step * static_cast<Float>(i));
  }

  insert_cylinder(params_.radius);

  assert(cylinders.size() == params_.steps);
  assert(temperatures.size() == params_.steps);
  assert(intensities.size() == params_.steps);
  assert(attenuations.size() == params_.steps);

  if constexpr (kDebugLevel >= 1) {
    std::cout << "[SolidCylinder]\n"
                 "Cylinders:\n";
    for (auto& cylinder : cylinders) {
      std::cout << cylinder.center() << ' ' << std::sqrt(cylinder.radius2())
                << '\n';
    }

    std::cout << "T:\n";
    for (const auto t : temperatures) {
      std::cout << t << '\n';
    }

    std::cout << "I:\n";
    for (const auto i : intensities) {
      std::cout << i << '\n';
    }
  }
}

WorkerResult SolidCylinder::SolveDir(const WorkerParams& params) const {
  SolidCylinderWorker worker{*this};
  return worker.SolveDir(params);
}

Float SolidCylinder::CalculateIntensity(Vec3 initial_pos,
                                        Vec3 dir,
                                        std::size_t sphere_points) const {
  SolidCylinderWorker worker{*this};
  return worker.CalculateIntensity(initial_pos, dir, sphere_points);
}
