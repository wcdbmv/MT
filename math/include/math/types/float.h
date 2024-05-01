#pragma once

using Float = double;

[[nodiscard]] constexpr Float operator""_F(long double value) noexcept {
  return static_cast<Float>(value);
}
