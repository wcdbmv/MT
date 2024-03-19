#pragma once

#include "base/float.h"
#include "math/linalg/vector.h"

using Vector3F = Vector<3, Float>;

inline constexpr auto kOrigin = Vector3F{};
inline constexpr auto kUnitX = Vector3F{1, 0, 0};
inline constexpr auto kUnitY = Vector3F{0, 1, 0};
inline constexpr auto kUnitZ = Vector3F{0, 0, 1};
