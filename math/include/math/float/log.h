#pragma once

#include <cmath>
#include <concepts>

#include "base/config/float.h"

template <std::floating_point>
struct LogTraits {};

template <>
struct LogTraits<float> {
  [[nodiscard]] static float Log(float x) noexcept { return std::logf(x); }
};

template <>
struct LogTraits<double> {
  [[nodiscard]] static double Log(double x) noexcept { return std::log(x); }
};

[[nodiscard]] inline Float Log(Float x) noexcept {
  return LogTraits<Float>::Log(x);
}
