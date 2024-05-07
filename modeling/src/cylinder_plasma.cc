#include "modeling/cylinder_plasma.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "base/erase_remove_if.h"
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

namespace {

inline constexpr auto kTW = 2'000.0_F;  // К.
inline constexpr auto kR = 0.35_F;      // см.
inline constexpr auto kH = 1.0_F;       // см.

inline constexpr auto kT0 = 12'000.0_F;  // К.
inline constexpr auto kM = 8;            // 2-8.
inline constexpr auto kRho = 0.98_F;     // 0-1.

/// Температура.
[[nodiscard]] constexpr Float T(Float z) noexcept {
  assert(0 <= z && z <= 1);
#ifndef CONSTANT_TEMPERATURE
  return kT0 + (kTW - kT0) * FastPow<kM>(z);
#else
  IgnoreUnused(z);
  return kT0;
#endif
}

}  // namespace

constexpr auto kStep = 0.00875_F;  // R / 40;

[[nodiscard]] std::size_t NPlasma() noexcept {
  return static_cast<size_t>(std::round(kR / kStep));
}

// TODO(a.kerimov): Выяснить, что происходит при 400000+ и CONSTANT_TEMPERATURE
constexpr auto kSpherePoints = 1000;

// TODO(a.kerimov): Написать тесты.

constexpr auto kOrigin = Vec3{};

}  // namespace

class CylinderPlasma::Impl {
 public:
  Impl(Float nu, Float d_nu)
      : plasma_{{.center = kOrigin,
                 .radius = kR,
                 .steps = NPlasma(),
                 .refractive_index = params::plasma::kEta,
                 .refractive_index_external = params::air::kEta,
                 .mirror = kRho},
                T,
                [nu, d_nu](Float t) { return func::I(nu, d_nu, t); },
                [nu](Float t) {
                  return params::plasma::AbsortionCoefficient(nu, t);
                }} {
    InitDirs();
  }

  Result Solve() {
    Result r;
    r.absorbed_plasma = std::vector<Float>(NPlasma());

    constexpr auto kInitialPos = Vec3{kR, 0, kH / 2};

    std::vector<Float> is;
    is.reserve(dirs_.size());
    Float max_intensity{};
    for (const auto dir : dirs_) {
      const auto i =
          plasma_.CalculateIntensity(kInitialPos, dir, kSpherePoints);
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
          kRho * intensity_before_reflection;
      r.absorbed_mirror +=
          intensity_before_reflection - intensity_after_reflection;

      auto res = plasma_.SolveDir({kInitialPos, dir, intensity_after_reflection,
                                   0.000001 * max_intensity});
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

    return r;
  }

 private:
  void InitDirs() {
    dirs_ = FibonacciSphere(kSpherePoints);
    EraseRemoveIf(dirs_, [](Vec3 dir) { return dir.x() <= 0; });
    for (const auto dir : dirs_) {
      DEBUG_OUT << dir << '\n';
    }
  }

  SolidCylinder plasma_;
  std::vector<Vec3> dirs_;
};

CylinderPlasma::CylinderPlasma(Float nu, Float d_nu) : pimpl_{nu, d_nu} {}

CylinderPlasma::~CylinderPlasma() = default;

auto CylinderPlasma::Solve() -> Result {
  return pimpl_->Solve();
}
