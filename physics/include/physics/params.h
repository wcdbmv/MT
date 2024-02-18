#pragma once

#include <cassert>
#include <cmath>

#include "base/float.h"
#include "math/fast_pow.h"

namespace params {

inline constexpr auto T_0 = static_cast<Float>(10'000);  // К.
inline constexpr auto T_w = static_cast<Float>(2'000);   // К.
inline constexpr auto m = 4;                             // 4-8

inline constexpr auto R = static_cast<Float>(0.35);     // см.
inline constexpr auto delta = static_cast<Float>(0.1);  // см.
inline constexpr auto R_1 = R + delta;                  // см.
inline constexpr auto H = static_cast<Float>(1);  // см.

/// Температура.
[[nodiscard]] constexpr Float T(const Float z) noexcept {
  assert(0 <= z);
  if (z <= 1) {
    return T_0 + (T_w - T_0) * FastPow<m>(z);
  }
  assert(z <= params::R_1 / params::R);
  constexpr auto a = static_cast<Float>(78848.21035368084688136380896708527);
  constexpr auto b = static_cast<Float>(3.674377435745371909154547914447763);
  return a * std::exp(-b * z);
}

/// Коэффициент отражения покрытия.
inline constexpr auto rho = static_cast<Float>(0.9);

/// Коэффициент поглощения плазмы.
[[nodiscard]] constexpr Float k_plasma(const Float T) noexcept {
  return static_cast<Float>(0.1) * Sqr(T / 2000);
}

/// Коэффициент поглощения кварца.
[[nodiscard]] constexpr Float k_quartz(const Float T) noexcept {
  return static_cast<Float>(0.001) * std::pow(T / 300, static_cast<Float>(1.5));
}

}  // namespace params
