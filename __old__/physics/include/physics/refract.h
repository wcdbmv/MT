#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/vector3f.h"

[[nodiscard]] Vector3F Refract(Vector3F I, Vector3F N, Float eta_i, Float eta_t)
    NOEXCEPT_RELEASE;
[[nodiscard]] Vector3F RefractEx(Vector3F I,
                                 Vector3F N,
                                 Float mu,
                                 Float cos_i,
                                 Float g) NOEXCEPT_RELEASE;
