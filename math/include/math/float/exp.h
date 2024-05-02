#pragma once

#include <cmath>
#include <concepts>

#include "base/config/float.h"

template <std::floating_point>
struct ExpTraits {};

template <>
struct ExpTraits<float> {
  [[nodiscard]] static float Exp(float x) noexcept { return std::expf(x); }
};

template <>
struct ExpTraits<double> {
  [[nodiscard]] static double Exp(double x) noexcept { return std::exp(x); }
};

[[nodiscard]] inline Float Exp(Float x) noexcept {
  return ExpTraits<Float>::Exp(x);
}
