#pragma once

#include <concepts>

#include "base/config/float.h"

template <std::floating_point>
struct EpsTraits {};

template <>
struct EpsTraits<float> {
  static constexpr auto kEps = 1E-4F;
};

template <>
struct EpsTraits<double> {
  static constexpr auto kEps = 1E-12;
};

inline constexpr auto kEps = EpsTraits<Float>::kEps;
