#pragma once

#ifdef MT_USE_DOUBLE
using Float = double;
#else
using Float = float;
#endif

[[nodiscard]] constexpr Float operator""_F(long double value) noexcept {
  return static_cast<Float>(value);
}
