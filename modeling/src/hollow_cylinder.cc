#include "modeling/hollow_cylinder.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "base/config/float.h"
#include "base/ignore_unused.h"
#include "math/float/eps.h"
#include "math/float/compare.h"
#include "math/float/exp.h"
#include "math/float/sqrt.h"
#include "math/linalg/vector.h"
#include "math/linalg/vector_io.h"
#include "ray_tracing/cylinder_z_infinite.h"
#include "ray_tracing/utils.h"

namespace {

// TODO(a.kerimov): Написать тесты.

constexpr auto kDebugLevel = 0;  // 0-2.

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

    pos_ = params.pos;
    dir_ = params.dir;
    // TODO(a.kerimov): Revive geogebra output (see git history).

    use_prev_ = params.use_prev;
    assert(!use_prev_);

    for ([[maybe_unused]] size_t i = 0; intensity > params.intensity_end; ++i) {
      if constexpr (kDebugLevel >= 2) {
        std::cout << "[HollowCylinder iteration=" << i
                  << ", intensity=" << intensity << ", use_prev=" << use_prev_
                  << "]\n";
      }

      Intersect();

      const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
      const auto dr = Vec3::Distance(prev_pos_, pos_);
      const auto k = c_.attenuations[idx];
      const auto exp = Exp(-k * dr);
      const auto prev_intensity = intensity;
      intensity *= exp;
      result.absorbed[idx] += prev_intensity - intensity;
      assert(idx != 0);

      if constexpr (kDebugLevel >= 2) {
        std::cout << "NEW POS: " << pos_ << " [ti=" << t_min_idx_
                  << "][ci=" << current_cylinder_idx_ << "][dir=" << dir_
                  << "]\n";
      }

      const auto outward = current_cylinder_idx_ == border_idx;
      if (outward || current_cylinder_idx_ == 0) {
        assert(outward == !use_prev_);
        const auto& p = c_.params();
        const auto eta_t =
            outward ? p.refractive_index_external : p.refractive_index_internal;
        const auto mirror = outward ? p.mirror_external : p.mirror_internal;
        const auto res = c_.cylinders[current_cylinder_idx_].Refract(
            pos_, dir_, p.refractive_index, eta_t, mirror, outward);

        if (res.T > 0) {
          if (const auto new_i = intensity * res.T;
              new_i > params.intensity_end) {
            result.released_rays.push_back(
                {pos_, res.refracted, new_i, params.intensity_end, use_prev_});
          } else if (outward) {
            result.absorbed_at_the_border += new_i;
          } else {
            result.absorbed[0] += new_i;
          }
        }

        assert(res.R > 0);
        dir_ = res.reflected;
        intensity *= res.R;
        use_prev_ = outward;

        if constexpr (kDebugLevel >= 2) {
          std::cout << "REFLECT, new dir " << dir_ << '\n';
        }
      }
    }

    if constexpr (kDebugLevel >= 2) {
      std::cout << "[HollowCylinder iteration=LAST, intensity=" << intensity
                << "]\n";
    }

    Intersect();

    const auto idx = use_prev_ ? prev_cylinder_idx_ : current_cylinder_idx_;
    result.absorbed[idx] += intensity;
    assert(idx != 0);

    return result;
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
    ts_[kIdxCurrCylinder] = IsZero(t) ? -1 : t;

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
    // TODO(a.kerimov): Fix all Point-s.
    pos_ = pos_ + ts_[t_min_idx_] * dir_;

    prev_cylinder_idx_ = current_cylinder_idx_;

    if (t_min_idx_ == kIdxPrevCylinder) {
      --current_cylinder_idx_;
    } else if (t_min_idx_ == kIdxNextCylinder) {
      ++current_cylinder_idx_;
    }

    use_prev_ = current_cylinder_idx_ <= prev_cylinder_idx_;
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const HollowCylinder& c_;

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
    const auto t = temperature((radius - step / 2) / params_.radius_min);
    temperatures.emplace_back(t);
    intensities.emplace_back(intensity(t));
    attenuations.emplace_back(attenuation(t));
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

  if constexpr (kDebugLevel >= 1) {
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
  }
}

WorkerResult HollowCylinder::SolveDir(const WorkerParams& params) const {
  HollowCylinderWorker worker{*this};
  return worker.SolveDir(params);
}
