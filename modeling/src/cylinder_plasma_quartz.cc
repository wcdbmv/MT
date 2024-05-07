#include "modeling/cylinder_plasma_quartz.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <vector>

#include "base/erase_remove_if.h"
#include "math/fast_pow.h"
#include "math/float/exp.h"
#include "math/linalg/vector.h"
#include "modeling/fibonacci_sphere.h"
#include "modeling/hollow_cylinder.h"
#include "modeling/solid_cylinder.h"
#include "modeling/worker.h"
#include "physics/params/air.h"
#include "physics/params/mirror.h"
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

namespace {

inline constexpr auto kT0 = 10'000.0_F;  // К.
inline constexpr auto kTW = 2'000.0_F;   // К.
inline constexpr auto kM = 4;            // 4-8

inline constexpr auto kR = 0.35_F;        // см.
inline constexpr auto kDelta = 0.1_F;     // см.
inline constexpr auto kR1 = kR + kDelta;  // см.
inline constexpr auto kH = 1.0_F;         // см.

/// Температура.
[[nodiscard]] constexpr Float T(Float z) noexcept {
  assert(0 <= z);
#ifndef CONSTANT_TEMPERATURE
  if (z <= 1) {
    return kT0 + (kTW - kT0) * FastPow<kM>(z);
  }
  assert(z <= kR1 / kR);
  constexpr auto kA = 78848.21035368084688136380896708527_F;
  constexpr auto kB = 3.674377435745371909154547914447763_F;
  return kA * Exp(-kB * z);
#else
  IgnoreUnused(z);
  return kT0;
#endif
}

}  // namespace

constexpr auto kStep = 0.025_F;

[[nodiscard]] std::size_t NPlasma() noexcept {
  return static_cast<size_t>(std::round(kR / kStep));
}

[[nodiscard]] std::size_t NQuartz() noexcept {
  return static_cast<size_t>(std::round((kR1 - kR) / kStep));
}

constexpr auto kMirrorR = kZero;
constexpr auto kMirrorR1 = params::mirror::kRho;

// TODO(a.kerimov): Выяснить, что происходит при 400000+ и CONSTANT_TEMPERATURE
constexpr auto kSpherePoints = 1000;

// TODO(a.kerimov): Написать тесты.

constexpr auto kOrigin = Vec3{};

}  // namespace

class CylinderPlasmaQuartz::Impl {
 public:
  Impl(Float nu, Float d_nu)
      : plasma_{{.center = kOrigin,
                 .radius = kR,
                 .steps = NPlasma(),
                 .refractive_index = params::plasma::kEta,
                 .refractive_index_external = params::quartz::kEta,
                 .mirror = kMirrorR},
                T,
                [nu, d_nu](Float t) { return func::I(nu, d_nu, t); },
                [nu](Float t) {
                  return params::plasma::AbsortionCoefficient(nu, t);
                }},
        quartz_{{.center = kOrigin,
                 .radius_min = kR,
                 .radius_max = kR1,
                 .steps = NQuartz(),
                 .refractive_index = params::quartz::kEta,
                 .refractive_index_internal = params::plasma::kEta,
                 .refractive_index_external = params::air::kEta,
                 .mirror_internal = kMirrorR,
                 .mirror_external = kMirrorR1},
                T,
                [nu, d_nu](Float t) { return func::I(nu, d_nu, t); },
                [nu](Float t) {
                  return params::quartz::AbsortionCoefficient(nu, t);
                }} {
    InitDirs();
  }

  Result Solve() {
    Result r;
    r.absorbed_plasma = std::vector<Float>(NPlasma());
    r.absorbed_quartz = std::vector<Float>(NQuartz() + 1);
    std::vector<WorkerParams> wait_plasma;
    std::vector<WorkerParams> wait_quartz;

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
    for (const auto dir : dirs_) {
      auto res = quartz_.SolveDir(
          {kInitialPos, dir, is[jj], 0.000001 * max_intensity});
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
    dirs_ = FibonacciSphere(kSpherePoints);
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

  SolidCylinder plasma_;
  HollowCylinder quartz_;
  std::vector<Vec3> dirs_;
};

CylinderPlasmaQuartz::CylinderPlasmaQuartz(Float nu, Float d_nu)
    : pimpl_{nu, d_nu} {}

CylinderPlasmaQuartz::~CylinderPlasmaQuartz() = default;

auto CylinderPlasmaQuartz::Solve() -> Result {
  return pimpl_->Solve();
}
