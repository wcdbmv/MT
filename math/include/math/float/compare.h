#pragma once

#include <algorithm>
#include <cmath>

#include "base/config/float.h"
#include "math/float/eps.h"

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
