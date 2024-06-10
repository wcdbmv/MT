#pragma once

#include "base/config/float.h"

namespace params::plasma {

/// Показатель преломления плазмы.
inline constexpr auto kEta = 1.0_F;

/// Коэффициент поглощения плазмы.
[[nodiscard]] Float AbsorptionCoefficient(Float nu, Float t) noexcept;

[[nodiscard]] Float AbsorptionCoefficientFromTable(Float nu, Float t) noexcept;

}  // namespace params::plasma
