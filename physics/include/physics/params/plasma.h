#pragma once

#include "base/config/float.h"

namespace params::plasma {

/// Показатель преломления плазмы.
inline constexpr auto kEta = 1.0_F;

/// Коэффициент поглощения плазмы.
[[nodiscard]] Float AbsortionCoefficient(Float nu, Float t) noexcept;

[[nodiscard]] Float AbsortionCoefficientFromTable(Float nu, Float t) noexcept;

}  // namespace params::plasma
