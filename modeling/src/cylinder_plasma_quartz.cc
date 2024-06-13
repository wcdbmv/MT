#include "modeling/cylinder_plasma_quartz.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <type_traits>
#include <vector>

#include "base/erase_remove_if.h"
#include "math/consts/pi.h"
#include "math/fast_pow.h"
#include "math/linalg/vector.h"
#include "modeling/fibonacci_sphere.h"
#include "modeling/hollow_cylinder.h"
#include "modeling/solid_cylinder.h"
#include "modeling/worker.h"
#include "physics/params/air.h"
#include "physics/params/plasma.h"
#include "physics/params/quartz.h"
#include "physics/plancks_law.h"

// #define ENABLE_DEBUG_OUTPUT
// #define ENABLE_GEOGEBRA_OUTPUT
// #define ENABLE_GEOGEBRA_OUTPUT_2D
// #define ENABLE_GEOGEBRA_OUTPUT_SPHERE

#ifdef ENABLE_DEBUG_OUTPUT
#include "math/linalg/vector_io.h"
#endif

namespace {

struct DummyOut {};
[[maybe_unused]] DummyOut gDummyOut;  // NOLINT

template <typename T>
DummyOut& operator<<(DummyOut& dummy_out, T&&) noexcept {  // NOLINT
  return dummy_out;
}

#ifdef ENABLE_DEBUG_OUTPUT
#include <iostream>
#define DEBUG_OUT std::cout
#else
#define DEBUG_OUT gDummyOut
#endif

#ifdef ENABLE_GEOGEBRA_OUTPUT
#include <iostream>
#define GEOGEBRA_OUT std::cout
#else
#define GEOGEBRA_OUT gDummyOut
#endif

#ifdef ENABLE_GEOGEBRA_OUTPUT_2D
#include <iostream>
#define GEOGEBRA_OUT_2D std::cout
#else
#define GEOGEBRA_OUT_2D gDummyOut
#endif

// TODO(a.kerimov): Выяснить, что происходит при 400000+ и CONSTANT_TEMPERATURE
// TODO(a.kerimov): Написать тесты.

constexpr auto kOrigin = Vec3{};

}  // namespace

class CylinderPlasmaQuartz::Impl {
 public:
  Impl(const Params& params)
      : params_{params},
        b_{params.r / params.delta * std::log(params.tw / params.t1)},
        a_{params.tw * std::exp(b_)},
        sphere_points_{params_.n_meridian * params_.n_latitude / 10},
        plasma_{
            {.center = kOrigin,
             .radius = params.r,
             .steps = params.n_plasma,
             .refractive_index = params.eta_plasma,
             .refractive_index_external = params.eta_quartz,
             .mirror = kZero},
            [this](Float z) {
              assert(0 <= z && z <= 1);
              return params_.t0 +
                     (params_.tw - params_.t0) * FastPow(z, params_.m);
            },
            [this](Float t) { return func::I(params_.nu, params_.d_nu, t); },
            [this](Float t) {
              return params::plasma::AbsorptionCoefficient(params_.nu, t);
            }},
        quartz_{
            {.center = kOrigin,
             .radius_min = params.r,
             .radius_max = params.r + params.delta,
             .steps = params.n_quartz,
             .refractive_index = params.eta_quartz,
             .refractive_index_internal = params.eta_plasma,
             .refractive_index_external = params::air::kEta,
             .mirror_internal = kZero,
             .mirror_external = params.rho},
            [this](Float z) {
              if (z <= 1) {
                return params_.tw;
              }
              assert(z <= 1 + params_.delta / params_.r);
              return a_ * std::exp(-b_ * z);
            },
            [this](Float t) { return func::I(params_.nu, params_.d_nu, t); },
            [this](Float t) {
              return params::quartz::AbsorptionCoefficient(params_.nu, t);
            }} {
    InitDirs();
  }

  Result Solve() {
    Result r{
        .absorbed_plasma = std::vector<Float>(params_.n_plasma),
        .absorbed_plasma3 = std::vector<Float>(params_.n_plasma),
        .absorbed_quartz = std::vector<Float>(params_.n_quartz + 1),
        .absorbed_quartz3 = std::vector<Float>(params_.n_quartz + 1),
    };
    std::vector<WorkerParams> wait_plasma;
    std::vector<WorkerParams> wait_quartz;

    const auto initial_pos = Vec3{params_.r, 0, 0};

    std::vector<Float> is;
    is.reserve(dirs_.size());
    Float max_intensity{};
    for (const auto dir : dirs_) {
      const auto i =
          plasma_.CalculateIntensity(initial_pos, dir, sphere_points_);
      is.push_back(i);
      r.intensity_all += i;
      max_intensity = std::max(i, max_intensity);
    }

    std::size_t jj = 0;
    for (const auto dir : dirs_) {
      auto res = quartz_.SolveDir(
          {initial_pos, dir, is[jj], params_.i_crit * max_intensity});
      r.absorbed_mirror += res.absorbed_at_the_border;
      static_assert(std::is_trivially_copyable_v<WorkerParams>);
      for (auto released : res.released_rays) {
        if (!released.use_prev) {
          r.absorbed_mirror += released.intensity;
        } else {
          wait_plasma.push_back(released);
        }
      }

      assert(r.absorbed_quartz.size() == res.absorbed.size());
      DEBUG_OUT << "ABSORBED:\n";
      for (size_t i = 0; i < res.absorbed.size(); ++i) {
        r.absorbed_quartz[i] += res.absorbed[i];
        DEBUG_OUT << res.absorbed[i] << '\n';
      }

      ++jj;
    }

    DEBUG_OUT << "[[wait]]\n";
    // NOLINTNEXTLINE(modernize-loop-convert)
    while (!wait_plasma.empty() || !wait_quartz.empty()) {
      while (!wait_plasma.empty()) {
        auto last = wait_plasma.back();
        wait_plasma.pop_back();

        auto res = plasma_.SolveDir(last);
        r.absorbed_quartz[1] += res.absorbed_at_the_border;
        for (auto released : res.released_rays) {
          assert(!released.use_prev);
          wait_quartz.push_back(released);
        }

        assert(r.absorbed_plasma.size() == res.absorbed.size());
        DEBUG_OUT << "ABSORBED:\n";
        for (size_t j = 0; j < res.absorbed.size(); ++j) {
          r.absorbed_plasma[j] += res.absorbed[j];
          DEBUG_OUT << res.absorbed[j] << '\n';
        }
      }

      while (!wait_quartz.empty()) {
        auto last = wait_quartz.back();
        wait_quartz.pop_back();

        auto res = quartz_.SolveDir(last);
        r.absorbed_mirror += res.absorbed_at_the_border;
        for (auto released : res.released_rays) {
          if (!released.use_prev) {
            r.absorbed_mirror += released.intensity;
          } else {
            wait_plasma.push_back(released);
          }
        }

        assert(r.absorbed_quartz.size() == res.absorbed.size());
        DEBUG_OUT << "ABSORBED:\n";
        for (size_t i = 0; i < res.absorbed.size(); ++i) {
          r.absorbed_quartz[i] += res.absorbed[i];
          DEBUG_OUT << res.absorbed[i] << '\n';
        }
      }
    }

    // !!!!!!!!!!!!!!
    r.absorbed_plasma.back() += r.absorbed_quartz.front();
    r.absorbed_quartz.front() = 0;

    if (params_.n_quartz > 2) {
      const auto quartz_first =
          r.absorbed_quartz[2] +
          std::abs(r.absorbed_quartz[2] - r.absorbed_quartz[3]);
      auto d_quartz_linear = r.absorbed_quartz[1] - quartz_first;
      if (d_quartz_linear > 0) {
        r.absorbed_quartz[1] = quartz_first;

        const auto quartz_sum = std::accumulate(r.absorbed_quartz.begin() + 1,
                                                r.absorbed_quartz.end(), kZero);
        const auto d_quartz = d_quartz_linear / quartz_sum + 1;

        for (std::size_t i = 0; i < params_.n_quartz; ++i) {
          r.absorbed_quartz[i + 1] *= d_quartz;
        }
      }

      const auto quartz_last = 2 * r.absorbed_quartz[params_.n_quartz - 1] -
                               r.absorbed_quartz[params_.n_quartz - 2];
      d_quartz_linear = r.absorbed_quartz.back() - quartz_last;
      if (d_quartz_linear > 0) {
        r.absorbed_quartz.back() = quartz_last;

        r.absorbed_mirror += d_quartz_linear;
      }
    }

    auto plasma_last = 2 * r.absorbed_plasma[params_.n_plasma - 2] -
                             r.absorbed_plasma[params_.n_plasma - 3];
    if (plasma_last < 0) {
      plasma_last = r.absorbed_plasma[params_.n_plasma - 2] * 0.95;
    }
    const auto d_plasma_linear = r.absorbed_plasma.back() - plasma_last;
    if (d_plasma_linear > 0) {
      r.absorbed_plasma.back() = plasma_last;

      const auto plasma_sum = std::accumulate(r.absorbed_plasma.begin(),
                                              r.absorbed_plasma.end(), kZero);
      const auto d_plasma = d_plasma_linear / plasma_sum + 1;

      for (auto& ap : r.absorbed_plasma) {
        ap *= d_plasma;
      }
    }
    // !!!!!!!!!!!!!!

    const auto step_plasma = params_.r / static_cast<Float>(params_.n_plasma);
    Float r_avg = kZero;
    for (std::size_t i = 0; i < params_.n_plasma; ++i) {
      r_avg = step_plasma * (static_cast<Float>(i) + 0.5_F);
      r.absorbed_plasma3[i] = 2 * consts::kPi * r.absorbed_plasma[i] / r_avg;
    }

    //    r.absorbed_quartz3.front() =
    //        2 * consts::kPi * r.absorbed_plasma.front() / r_avg;

    const auto step_quartz =
        params_.delta / static_cast<Float>(params_.n_quartz);
    for (std::size_t i = 0; i < params_.n_quartz; ++i) {
      r_avg = params_.r + step_quartz * (static_cast<Float>(i) + 0.5_F);
      r.absorbed_quartz3[i + 1] =
          2 * consts::kPi * r.absorbed_quartz[i + 1] / r_avg;
    }

#ifndef XENON_TABLE_COEFFICIENT
    Float total_plasma = 0;
    std::cout << "TOTAL ABSORBED PLASMA:\n";
    for (const auto i : r.absorbed_plasma) {
      std::cout << i << '\n';
      total_plasma += i;
    }
    // TODO(a.kerimov): First quartz is the last plasma.
    Float total_quartz = 0;
    std::cout << "TOTAL ABSORBED QUARTZ:\n";
    for (const auto i : r.absorbed_quartz) {
      std::cout << i << '\n';
      total_quartz += i;
    }

    std::cout << "ABSORBED QUARTZ: " << r.absorbed_mirror << '\n';
    std::cout << "SUM: " << total_plasma + total_quartz + r.absorbed_mirror
              << '\n';
    std::cout << "INTENSITY ALL: " << r.intensity_all << '\n';
#endif

    return r;
  }

 private:
  void InitDirs() {
    dirs_ = FibonacciSphere(sphere_points_);
    EraseRemoveIf(dirs_, [](Vec3 dir) { return dir.x() <= 0; });
    for (const auto dir : dirs_) {
      DEBUG_OUT << dir << '\n';
#ifdef ENABLE_GEOGEBRA_OUTPUT_SPHERE
      static int i = 0;
      std::cout << geogebra::Point3D(std::format("P{}", i), dir);
      ++i;
#endif
    }
  }

  Params params_;
  Float b_;
  Float a_;
  std::size_t sphere_points_;

  SolidCylinder plasma_;
  HollowCylinder quartz_;
  std::vector<Vec3> dirs_;
};

CylinderPlasmaQuartz::CylinderPlasmaQuartz(const Params& params)
    : pimpl_{params} {}

CylinderPlasmaQuartz::~CylinderPlasmaQuartz() = default;

auto CylinderPlasmaQuartz::Solve() -> Result {
  return pimpl_->Solve();
}
