#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>

#include "base/float.h"

inline constexpr auto kEps = std::numeric_limits<Float>::epsilon();

template <std::floating_point T>
constexpr bool IsZero(
    const T value,
    const T eps = std::numeric_limits<T>::epsilon()) noexcept {
  return std::abs(value) < eps;
}

template <std::floating_point T>
constexpr bool IsEqual(
    const T x,
    const T y,
    const T eps = std::numeric_limits<T>::epsilon()) noexcept {
  const auto diff = std::abs(x - y);
  if (diff <= eps) {
    return true;
  }

  return diff < std::max(std::abs(x), std::abs(y)) * eps;
}
