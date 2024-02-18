#pragma once

#include <cmath>

#include "base/float.h"
#include "math/consts/pi.h"
#include "math/fast_pow.h"
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
/// @param nu Частота излучения.
/// @param T  Абсолютная температура
/// @returns  Спектральная плотность излучения.
[[nodiscard]] constexpr Float u_nu(const Float nu, const Float T) noexcept {
  return 8 * consts::pi * consts::h * Cube(nu) /
         (Cube(consts::c) * (std::exp(consts::h * nu / (consts::k * T)) - 1));
}

}  // namespace func
