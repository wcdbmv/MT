#pragma once

#include <cmath>
#include <concepts>

template <std::floating_point T>
struct SqrtTraits {};

template <>
struct SqrtTraits<float> {
  [[nodiscard]] static float Sqrt(float x) noexcept { return std::sqrtf(x); }
};

template <>
struct SqrtTraits<double> {
  [[nodiscard]] static double Sqrt(double x) noexcept { return std::sqrt(x); }
};

template <std::floating_point T>
[[nodiscard]] T Sqrt(T x) noexcept {
  return SqrtTraits<T>::Sqrt(x);
}
