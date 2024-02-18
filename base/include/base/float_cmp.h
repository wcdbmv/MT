#pragma once

#include <cmath>
#include <limits>

#include "base/float.h"

inline constexpr auto Eps = std::numeric_limits<Float>::epsilon();
inline constexpr auto Inf = std::numeric_limits<Float>::infinity();

constexpr bool IsZero(const Float value) noexcept {
  return std::abs(value) < Eps;
}

constexpr bool IsEqual(const Float x, const Float y) noexcept {
  return IsZero(x - y);
}
