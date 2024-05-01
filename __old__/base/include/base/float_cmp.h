#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>

#include "base/float.h"

template <std::floating_point T>
struct EpsTraits {
  static constexpr auto kEps = static_cast<T>(1E-12);
};

inline constexpr auto kEps = EpsTraits<Float>::kEps;

template <std::floating_point T>
constexpr bool IsZero(
    const T value,
    const T eps = EpsTraits<T>::kEps) noexcept {
  return std::abs(value) < eps;
}

template <std::floating_point T>
constexpr bool IsEqual(
    const T x,
    const T y,
    const T eps = EpsTraits<T>::kEps) noexcept {
  const auto diff = std::abs(x - y);
  if (diff <= eps) {
    return true;
  }

  return diff < std::max(std::abs(x), std::abs(y)) * eps;
}
