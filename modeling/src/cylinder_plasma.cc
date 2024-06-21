#include "modeling/cylinder_plasma.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "base/erase_remove_if.h"
#include "math/consts/pi.h"
#include "math/fast_pow.h"
#include "math/linalg/vector.h"
#include "modeling/fibonacci_sphere.h"
#include "modeling/solid_cylinder.h"
#include "modeling/worker.h"
#include "physics/params/air.h"
#include "physics/params/plasma.h"
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

class CylinderPlasma::Impl {
 public:
  explicit Impl(const Params& params)
      : params_{params},
        sphere_points_{params_.n_meridian * params_.n_latitude},
        plasma_{
            {.center = kOrigin,
             .radius = params.r,
             .steps = params.n_plasma,
             .refractive_index = params::plasma::kEta,
             .refractive_index_external = params::air::kEta,
             .mirror = params.rho},
            [this](Float z) {
              assert(0 <= z && z <= 1);
              return params_.t0 +
                     (params_.tw - params_.t0) * FastPow(z, params_.m);
            },
            [this](Float t) { return func::I(params_.nu, params_.d_nu, t); },
            [this](Float t) {
              return params::plasma::AbsorptionCoefficient(params_.nu, t);
            }} {
    InitDirs();
  }

  Result Solve() {
    Result r{
        .absorbed_plasma = std::vector<Float>(params_.n_plasma),
        .absorbed_plasma3 = std::vector<Float>(params_.n_plasma),
    };

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
    for (const auto not_reflected_dir : dirs_) {
      // Reflect the mirror.
      auto dir = not_reflected_dir;
      dir.x() = -dir.x();

      const auto intensity_before_reflection = is[jj];
      const auto intensity_after_reflection =
          params_.rho * intensity_before_reflection;
      r.absorbed_mirror +=
          intensity_before_reflection - intensity_after_reflection;

      auto res = plasma_.SolveDir({initial_pos, dir, intensity_after_reflection,
                                   params_.i_crit * max_intensity});
      r.absorbed_mirror += res.absorbed_at_the_border;

      static_assert(std::is_trivially_copyable_v<WorkerParams>);
      for (auto released : res.released_rays) {
        assert(!released.use_prev);
        r.absorbed_mirror += released.intensity;
      }

      assert(r.absorbed_plasma.size() == res.absorbed.size());
      DEBUG_OUT << "ABSORBED:\n";
      for (size_t i = 0; i < res.absorbed.size(); ++i) {
        r.absorbed_plasma[i] += res.absorbed[i];
        DEBUG_OUT << res.absorbed[i] << '\n';
      }

      ++jj;
    }

    const auto step = params_.r / static_cast<Float>(params_.n_plasma);
    for (std::size_t i = 0; i < params_.n_plasma; ++i) {
      const auto r_avg = step * (static_cast<Float>(i) + 0.5_F);
      r.absorbed_plasma3[i] = 2 * consts::kPi * r.absorbed_plasma[i] / r_avg;
    }

    return r;
  }

 private:
  void InitDirs() {
    dirs_ = FibonacciSphere(sphere_points_);
    EraseRemoveIf(dirs_, [](Vec3 dir) { return dir.x() <= 0; });
    for (const auto dir : dirs_) {
      DEBUG_OUT << dir << '\n';
    }
  }

  CylinderPlasma::Params params_;
  std::size_t sphere_points_;

  SolidCylinder plasma_;
  std::vector<Vec3> dirs_;
};

CylinderPlasma::CylinderPlasma(const Params& params) : pimpl_{params} {}

CylinderPlasma::~CylinderPlasma() = default;

auto CylinderPlasma::Solve() -> Result {
  return pimpl_->Solve();
}
