#pragma once

#include <algorithm>
#include <cmath>

#include "math/types/float.h"

inline constexpr auto kEps = 1E-12_F;

[[nodiscard]] inline bool IsZero(Float value, Float eps = kEps) noexcept {
  return std::abs(value) < eps;
}

[[nodiscard]] inline bool IsEqual(Float x, Float y, Float eps = kEps) noexcept {
  const auto diff = std::abs(x - y);
  if (diff <= eps) {
    return true;
  }

  return diff < std::max(std::abs(x), std::abs(y)) * eps;
}
