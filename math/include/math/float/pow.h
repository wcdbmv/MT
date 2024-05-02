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

[[nodiscard]] constexpr Float Sqr(Float x) noexcept {
  return x * x;
}

[[nodiscard]] constexpr Float Cube(Float x) noexcept {
  return x * x * x;
}

[[nodiscard]] constexpr Float FourthPower(Float x) noexcept {
  return Sqr(Sqr(x));
}

[[nodiscard]] constexpr Float FifthPower(Float x) noexcept {
  return FourthPower(x) * x;
}

[[nodiscard]] constexpr Float SixthPower(Float x) noexcept {
  return Sqr(Cube(x));
}

[[nodiscard]] constexpr Float SeventhPower(Float x) noexcept {
  return SixthPower(x) * x;
}

[[nodiscard]] constexpr Float EightPower(Float x) noexcept {
  return Sqr(FourthPower(x));
}

template <int power>
[[nodiscard]] constexpr Float FastPow(Float x) noexcept {
  static_assert(2 <= power && power <= 8);

  if constexpr (power == 2) {
    return Sqr(x);
  }
  if constexpr (power == 3) {
    return Cube(x);
  }
  if constexpr (power == 4) {
    return FourthPower(x);
  }
  if constexpr (power == 5) {
    return FifthPower(x);
  }
  if constexpr (power == 6) {
    return SixthPower(x);
  }
  if constexpr (power == 7) {
    return SeventhPower(x);
  }
  if constexpr (power == 8) {
    return EightPower(x);
  }

  return {};
}
