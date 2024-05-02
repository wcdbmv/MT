#pragma once

#ifdef MT_USE_DOUBLE
using Float = double;
#else
using Float = float;
#endif

[[nodiscard]] constexpr Float operator""_F(long double value) noexcept {
  return static_cast<Float>(value);
}

inline constexpr auto kZero = 0.0_F;
inline constexpr auto kOne = 1.0_F;
