#pragma once

#include <cmath>
#include <concepts>

#include "base/config/float.h"

template <std::floating_point>
struct SqrtTraits {};

template <>
struct SqrtTraits<float> {
  [[nodiscard]] static float Sqrt(float x) noexcept { return std::sqrtf(x); }
};

template <>
struct SqrtTraits<double> {
  [[nodiscard]] static double Sqrt(double x) noexcept { return std::sqrt(x); }
};

[[nodiscard]] inline Float Sqrt(Float x) noexcept {
  return SqrtTraits<Float>::Sqrt(x);
}
