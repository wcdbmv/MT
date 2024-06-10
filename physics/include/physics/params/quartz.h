#pragma once

#include "base/config/float.h"
#include "base/ignore_unused.h"

namespace params::quartz {

/// Показатель преломления кварца.
inline constexpr auto kEta = 1.4585_F;

/// Коэффициент поглощения кварца.
[[nodiscard]] constexpr Float AbsorptionCoefficient(Float nu, Float t) noexcept {
#ifdef CONSTANT_TEMPERATURE
  IgnoreUnused(nu);
  IgnoreUnused(t);
  return 100.0_F;
#else
  // TODO(a.kerimov): Find quartz absortion coefficient table.
  IgnoreUnused(nu);
  return 0.001_F * std::pow(t / 300, 1.5_F);
#endif
}

}  // namespace params::quartz
