#pragma once

#include <cmath>
#include <concepts>

#include "base/config/float.h"

template <std::floating_point>
struct PowTraits {};

template <>
struct PowTraits<float> {
  [[nodiscard]] static float Pow(float base, float exp) noexcept {
    return std::powf(base, exp);
  }
};

template <>
struct PowTraits<double> {
  [[nodiscard]] static double Pow(double base, double exp) noexcept {
    return std::pow(base, exp);
  }
};

[[nodiscard]] inline Float Pow(Float base, Float exp) noexcept {
  return PowTraits<Float>::Pow(base, exp);
}
