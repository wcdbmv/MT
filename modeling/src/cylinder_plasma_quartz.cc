#include "modeling/cylinder_plasma_quartz.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "base/erase_remove_if.h"
#include "base/float.h"
#include "math/linalg/vector3f.h"
#include "modeling/fibonacci_sphere.h"
#include "modeling/hollow_cylinder.h"
#include "modeling/solid_cylinder.h"
#include "modeling/worker.h"
#include "physics/params.h"
#include "physics/plancks_law.h"

// #define ENABLE_DEBUG_OUTPUT
// #define ENABLE_GEOGEBRA_OUTPUT
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
#define DEBUG_OUT std::cout
#else
#define DEBUG_OUT gDummyOut
#endif

#ifdef ENABLE_GEOGEBRA_OUTPUT
#define GEOGEBRA_OUT std::cout
#else
#define GEOGEBRA_OUT gDummyOut
#endif

constexpr auto kStep = static_cast<Float>(0.05);
// NOLINTNEXTLINE(cert-err58-cpp)
const auto kPlasmaN = static_cast<size_t>(std::round(params::R / kStep));
// NOLINTNEXTLINE(cert-err58-cpp)
const auto kQuartzN =
    static_cast<size_t>(std::round((params::R_1 - params::R) / kStep));
// NOLINTNEXTLINE(cert-err58-cpp)
constexpr auto kMirrorR = static_cast<Float>(0);
constexpr auto kMirrorR1 = params::rho;

// TODO(a.kerimov): Выяснить, что происходит при 400000+ и CONSTANT_TEMPERATURE
constexpr auto kSpherePoints = 100;

// TODO(a.kerimov): Написать тесты.

}  // namespace

class CylinderPlasmaQuartz::Impl {
 public:
  Impl(const Float nu, const Float d_nu)
      : plasma_{{.center = kOrigin,
                 .radius = params::R,
                 .steps = kPlasmaN,
                 .refractive_index = params::n_plasma,
                 .refractive_index_external = params::n_quartz,
                 .mirror = kMirrorR},
                params::T,
                [nu, d_nu](const Float T) { return func::I(nu, d_nu, T); },
                [nu](const Float T) { return params::k_plasma(nu, T); }},
        quartz_{{.center = kOrigin,
                 .radius_min = params::R,
                 .radius_max = params::R_1,
                 .steps = kQuartzN,
                 .refractive_index = params::n_quartz,
                 .refractive_index_internal = params::n_plasma,
                 .refractive_index_external = params::n_quartz,
                 .mirror_internal = kMirrorR,
                 .mirror_external = kMirrorR1},
                params::T,
                [nu, d_nu](const Float T) { return func::I(nu, d_nu, T); },
                params::k_quartz} {
    InitDirs();
  }

  Result Solve() {
    Result r;
    r.absorbed_plasma = std::vector<Float>(kPlasmaN);
    r.absorbed_quartz = std::vector<Float>(kQuartzN + 1);
    std::vector<WorkerParams> wait_plasma;
    std::vector<WorkerParams> wait_quartz;

    for (const auto dir : dirs_) {
      constexpr auto kInitialPos = Vector3F{params::R, 0, params::H / 2};
      const auto I =
          plasma_.CalculateIntensity(kInitialPos, dir, kSpherePoints);
      r.intensity_all += I;

      auto res = quartz_.SolveDir({{kInitialPos, dir}, I, 0.000001 * I});
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
    EraseRemoveIf(dirs_, [](const Vector3F dir) { return dir.x() <= 0; });
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
  std::vector<Vector3F> dirs_;
};

CylinderPlasmaQuartz::CylinderPlasmaQuartz(const Float nu, const Float d_nu)
    : pimpl_{nu, d_nu} {}

CylinderPlasmaQuartz::~CylinderPlasmaQuartz() = default;

auto CylinderPlasmaQuartz::Solve() -> Result {
  return pimpl_->Solve();
}
