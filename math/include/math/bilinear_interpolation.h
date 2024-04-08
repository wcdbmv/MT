#pragma once

#include <cassert>

#include "base/float.h"

[[nodiscard]] constexpr Float BilinearInterpolation(const Float f00,
                                                    const Float f01,
                                                    const Float f10,
                                                    const Float f11,
                                                    const Float x,
                                                    const Float y) noexcept {
  assert(0 <= x && x <= 1);
  assert(0 <= y && y <= 1);

  return f00 * (1 - x) * (1 - y) +  //
         f10 * x * (1 - y) +        //
         f01 * (1 - x) * y +        //
         f11 * x * y;
}
