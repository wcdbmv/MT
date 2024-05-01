#pragma once

#include <cassert>
#include <cmath>

#include "base/float.h"
#include "math/exp.h"
#include "math/fast_pow.h"

#ifdef XENON_TABLE_COEFFICIENT
#include "base/float_cmp.h"
#include "math/log.h"
#include "physics/xenon_absorption_coefficient.h"
#endif

namespace params {

inline constexpr auto T_0 = static_cast<Float>(10'000);  // К.
inline constexpr auto T_w = static_cast<Float>(2'000);   // К.
inline constexpr auto m = 4;                             // 4-8

inline constexpr auto R = static_cast<Float>(0.35);     // см.
inline constexpr auto delta = static_cast<Float>(0.1);  // см.
inline constexpr auto R_1 = R + delta;                  // см.
inline constexpr auto H = static_cast<Float>(1);        // см.

/// Температура.
[[nodiscard]] constexpr Float T(const Float z) noexcept {
  assert(0 <= z);
#ifndef CONSTANT_TEMPERATURE
  if (z <= 1) {
    return T_0 + (T_w - T_0) * FastPow<m>(z);
  }
  assert(z <= params::R_1 / params::R);
  constexpr auto a = static_cast<Float>(78848.21035368084688136380896708527);
  constexpr auto b = static_cast<Float>(3.674377435745371909154547914447763);
  return a * Exp(-b * z);
#else
  (void)z;
  return T_0;
#endif
}

/// Коэффициент отражения покрытия.
inline constexpr auto rho = static_cast<Float>(0.95);

/// Коэффициент поглощения плазмы.
[[nodiscard]] constexpr Float k_plasma(const Float nu, const Float T) noexcept {
#if defined(CONSTANT_TEMPERATURE)
  (void)nu;
  (void)T;
  return static_cast<Float>(10000);
#elif defined(XENON_TABLE_COEFFICIENT)
  static_assert(kXenonTemperature.size() == 13);
  assert(IsEqual(kXenonTemperature[0], static_cast<Float>(2000)));
  assert(IsEqual(kXenonTemperature[1], static_cast<Float>(3000)));
  assert(IsEqual(kXenonTemperature[2], static_cast<Float>(4000)));
  assert(IsEqual(kXenonTemperature[3], static_cast<Float>(5000)));
  assert(IsEqual(kXenonTemperature[4], static_cast<Float>(6000)));
  assert(IsEqual(kXenonTemperature[5], static_cast<Float>(7000)));
  assert(IsEqual(kXenonTemperature[6], static_cast<Float>(8000)));
  assert(IsEqual(kXenonTemperature[7], static_cast<Float>(9000)));
  assert(IsEqual(kXenonTemperature[8], static_cast<Float>(10000)));
  assert(IsEqual(kXenonTemperature[9], static_cast<Float>(11000)));
  assert(IsEqual(kXenonTemperature[10], static_cast<Float>(12000)));
  assert(IsEqual(kXenonTemperature[11], static_cast<Float>(13000)));
  assert(IsEqual(kXenonTemperature[12], static_cast<Float>(14000)));

  assert(kXenonTemperature.front() <= T && T <= kXenonTemperature.back());
  const auto t_idx = static_cast<size_t>(T) / 1000 - 2;
  assert(t_idx <= 8);

  const auto* lower = std::ranges::lower_bound(kXenonFrequency, nu);
  assert(lower != kXenonFrequency.end());
  const auto nu_idx_p = std::distance(kXenonFrequency.begin(), lower);
  assert(1 <= nu_idx_p);
  const auto nu_idx = static_cast<size_t>(nu_idx_p - 1);
  assert(nu_idx + 1 < kXenonFrequency.size());

  const auto Tln = Log(T);
  const auto T0ln = Log(kXenonTemperature[t_idx]);
  const auto T1ln = Log(kXenonTemperature[t_idx + 1]);

  const auto f0 = kXenonAbsorptionCoefficient[t_idx][nu_idx];
  const auto f1 = kXenonAbsorptionCoefficient[t_idx + 1][nu_idx];
  const auto f0ln = Log(f0);
  const auto f1ln = Log(f1);

  const auto fln = f0ln + (Tln - T0ln) * (f1ln - f0ln) / (T1ln - T0ln);
  const auto f = Exp(fln);
  return f;

#else
  (void)nu;
  return static_cast<Float>(0.04) * Sqr(T / 2000);
#endif
}

/// Коэффициент поглощения кварца.
// [[nodiscard]] constexpr Float k_quartz(const Float T) noexcept {
// #ifndef CONSTANT_TEMPERATURE
//   return static_cast<Float>(0.001) * std::pow(T / 300,
//   static_cast<Float>(1.5));
// #else
//   (void)T;
//   return static_cast<Float>(100);
// #endif
// }
[[nodiscard]] constexpr Float k_quartz(const Float T) noexcept {
  return static_cast<Float>(0.001) * std::pow(T / 300, static_cast<Float>(1.5));
}

/// Показатель преломления плазмы.
inline constexpr auto n_plasma = static_cast<Float>(1);

/// Показатель преломления кварца.
inline constexpr auto n_quartz = static_cast<Float>(1.4585);
// TODO(a.kerimov): Make function.

/// Показатель преломления воздуха.
inline constexpr auto n_air = static_cast<Float>(1);

inline constexpr auto nu = static_cast<Float>(1e15);  // Гц.

}  // namespace params
