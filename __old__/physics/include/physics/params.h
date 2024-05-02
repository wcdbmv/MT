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

inline constexpr auto nu = static_cast<Float>(1e15);  // Гц.

}  // namespace params
