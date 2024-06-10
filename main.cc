#if 0  // NOLINT(readability-avoid-unconditional-preprocessor-if)
#include <array>
#include <cassert>
#include <cstddef>
#include <format>
#include <iostream>

#include "base/config/float.h"
#include "math/fast_pow.h"
#include "physics/params/plasma.h"
#include "physics/params/xenon_absorption_coefficient.h"

namespace {
inline constexpr auto kT0 = 10'000.0_F;  // К.
inline constexpr auto kTW = 2'000.0_F;   // К.
inline constexpr auto kM = 4;            // 2-8

inline constexpr auto kR = 0.35_F;  // см.
inline constexpr auto kN = static_cast<std::size_t>(40);

[[nodiscard]] constexpr Float T(Float z) noexcept {
  assert(0 <= z && z <= 1);
  return kT0 + (kTW - kT0) * FastPow<kM>(z);
}

}  // namespace

int main() {
  // Оптическая плотность tau = integral k * dr.
  std::cout << "range          tau      nu_min      nu_max          nu\n";
  for (std::size_t i = 0; i < kXenonTableRanges; ++i) {
    const auto nu_min = kXenonFrequency[i];
    const auto nu_max = kXenonFrequency[i + 1];
    const auto d_nu = nu_max - nu_min;
    const auto nu = nu_min + d_nu / 2;

    auto tau = 0.0_F;
    for (std::size_t j = 0; j < kN; ++j) {
      constexpr auto kStep = kR / kN;
      const auto r = kStep * static_cast<Float>(j + 1);
      const auto z = r / kR;

      const auto t = T(z);
      const auto k = params::plasma::AbsorptionCoefficientFromTable(nu, t);

      tau += k * kStep;
    }

    std::cout << std::format("{:5d} {:12.6f} {:11g} {:11g} {:11g}\n", i + 1, tau,
                             nu_min, nu_max, nu);
  }
}
#elif 1  // NOLINT(readability-avoid-unconditional-preprocessor-if)
#ifndef XENON_TABLE_COEFFICIENT
#error "Define XENON_TABLE_COEFFICIENT"
#endif
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "base/config/float.h"
#include "modeling/cylinder_plasma.h"
#include "physics/params/xenon_absorption_coefficient.h"

int main() {
  constexpr std::size_t kI = 169;
  const auto nu_min = kXenonFrequency[kI];
  const auto nu_max = kXenonFrequency[kI + 1];
  const auto d_nu = nu_max - nu_min;
  const auto nu_avg = nu_min + d_nu / 2;

  const auto params = CylinderPlasma::Params{
    .r = 0.35_F,
    .n_plasma = 40,

    .t0 = 10000.0_F,
    .tw = 2000.0_F,
    .m = 4,

    .rho = 0.95_F,

    .nu = nu_avg,
    .d_nu = d_nu,

    .n_meridian = 100,
    .n_latitude = 100,

    .n_threads = 4,
  };
  std::cout << "[[-----------------------------------------------------------"
               "-------------------]]\n"
               "[[i="
            << kI << ", nu_min=" << nu_min << ", nu_max=" << nu_max
            << ", d_nu=" << d_nu << ", nu_avg=" << nu_avg
            << "]]\n"
               "[[-----------------------------------------------------------"
               "-------------------]]\n";
  auto r = CylinderPlasma{params}.Solve();

  Float total_plasma = 0;
  std::cout << "TOTAL ABSORBED PLASMA:\n";
  for (const auto j : r.absorbed_plasma) {
    std::cout << j << '\n';
    total_plasma += j;
  }

  std::cout << /* "ABSORBED MIRROR: " << */ r.absorbed_mirror << '\n';
  std::cout << /* "SUM: " << */ total_plasma + r.absorbed_mirror << '\n';
  std::cout << "INTENSITY ALL: " << r.intensity_all << '\n';
}
#elif defined(XENON_TABLE_COEFFICIENT)
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "base/config/float.h"
#include "modeling/cylinder_plasma_quartz.h"
#include "physics/params/xenon_absorption_coefficient.h"

int main() {
  CylinderPlasmaQuartz::Result rr;

  for (size_t i = 0; i + 1 < kXenonFrequency.size(); ++i) {
    const auto nu_min = kXenonFrequency[i];
    const auto nu_max = kXenonFrequency[i + 1];
    const auto d_nu = nu_max - nu_min;
    const auto nu_avg = nu_min + d_nu / 2;
    std::cout << "[[-----------------------------------------------------------"
                 "-------------------]]\n"
                 "[[i="
              << i << ", nu_min=" << nu_min << ", nu_max=" << nu_max
              << ", d_nu=" << d_nu << ", nu_avg=" << nu_avg
              << "]]\n"
                 "[[-----------------------------------------------------------"
                 "-------------------]]\n";
    auto r = CylinderPlasmaQuartz{nu_avg, d_nu}.Solve();

    Float total_plasma = 0;
    std::cout << "TOTAL ABSORBED PLASMA:\n";
    for (const auto j : r.absorbed_plasma) {
      std::cout << j << '\n';
      total_plasma += j;
    }
    // TODO(a.kerimov): First quartz is the last plasma.
    Float total_quartz = 0;
    std::cout << "TOTAL ABSORBED QUARTZ:\n";
    for (const auto j : r.absorbed_quartz) {
      std::cout << j << '\n';
      total_quartz += j;
    }

    std::cout << "ABSORBED QUARTZ: " << r.absorbed_mirror << '\n';
    std::cout << "SUM: " << total_plasma + total_quartz + r.absorbed_mirror
              << '\n';
    std::cout << "INTENSITY ALL: " << r.intensity_all << '\n';

    if (i == 0) [[unlikely]] {
      rr = std::move(r);
    } else {
      assert(rr.absorbed_plasma.size() == r.absorbed_plasma.size());
      for (size_t j = 0; j < r.absorbed_plasma.size(); ++j) {
        rr.absorbed_plasma[j] += r.absorbed_plasma[j];
      }

      assert(rr.absorbed_quartz.size() == r.absorbed_quartz.size());
      for (size_t j = 0; j < r.absorbed_quartz.size(); ++j) {
        rr.absorbed_quartz[j] += r.absorbed_quartz[j];
      }

      rr.absorbed_mirror += r.absorbed_mirror;
      rr.intensity_all += r.intensity_all;
    }
  }

  Float total_plasma = 0;
  std::cout << "TOTAL ABSORBED PLASMA:\n";
  for (const auto j : rr.absorbed_plasma) {
    std::cout << j << '\n';
    total_plasma += j;
  }
  // TODO(a.kerimov): First quartz is the last plasma.
  Float total_quartz = 0;
  std::cout << "TOTAL ABSORBED QUARTZ:\n";
  for (const auto j : rr.absorbed_quartz) {
    std::cout << j << '\n';
    total_quartz += j;
  }

  std::cout << "ABSORBED QUARTZ: " << rr.absorbed_mirror << '\n';
  std::cout << "SUM: " << total_plasma + total_quartz + rr.absorbed_mirror
            << '\n';
  std::cout << "INTENSITY ALL: " << rr.intensity_all << '\n';
}
#else
#include "base/config/float.h"
#include "modeling/cylinder_plasma_quartz.h"

namespace {
constexpr auto kNu = 1E+15_F;
constexpr auto kDNu = 1E+15_F;
}  // namespace

int main() {
  CylinderPlasmaQuartz{kNu, kDNu}.Solve();
}
#endif
