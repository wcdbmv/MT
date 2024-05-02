#pragma once

#include "base/config/float.h"
#include "math/consts/pi.h"
#include "math/fast_pow.h"
#include "math/float/exp.h"
#include "physics/consts/boltzmann_constant.h"
#include "physics/consts/planck_constant.h"
#include "physics/consts/speed_of_light.h"

namespace func {

/// Формула Планка (закон излучения Планка).
///
/// Описывает спектральное распределение энергии электромагнитного излучения,
/// находящегося в тепловом равновесии с веществом при заданной температуре.
/// Идеализированной моделью равновесного излучения служит электромагнитное поле
/// внутри полости,  расположенной в нагретом веществе, при условии, что стенки
/// вещества непрозрачны для излучения. Спектр такого равновесного излучения
/// называют спектром излучения абсолютно чёрного тела.
/// @param nu Частота излучения [Гц].
/// @param t  Абсолютная температура [К].
/// @returns  Спектральная плотность излучения.
[[nodiscard]] constexpr Float UNu(Float nu, Float t) noexcept {
  return 8 * consts::kPi * consts::kPlanckConstant * Cube(nu) /
         (Cube(consts::kSpeedOfLightSm) *
          (Exp(consts::kPlanckConstant * nu / (consts::kBolzmannConstant * t)) -
           1));
}

/// Интенсивность.
///
/// 4πI/c = u_nu
[[nodiscard]] constexpr Float I(Float nu, Float d_nu, Float t) noexcept {
  return 2 * consts::kPlanckConstant * Cube(nu) * d_nu /
         (Sqr(consts::kSpeedOfLightSm) *
          (Exp(consts::kPlanckConstant * nu / (consts::kBolzmannConstant * t)) -
           1));
}

}  // namespace func
