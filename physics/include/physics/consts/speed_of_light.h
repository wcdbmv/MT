#pragma once

#include "base/float.h"

namespace consts {

/// Скорость света в вакууме.
inline constexpr auto c = static_cast<Float>(299'792'458);  // м/с.

inline constexpr auto c_sm = 100 * c;  // см/c.

}  // namespace consts
