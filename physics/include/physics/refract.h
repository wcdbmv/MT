#pragma once

#include "base/config/float.h"
#include "base/config/noexcept_release.h"
#include "math/linalg/vector.h"

[[nodiscard]] Vec3 Refract(Vec3 incident, Vec3 normal, Float eta_i, Float eta_t)
    MT_NOEXCEPT_RELEASE;
[[nodiscard]] Vec3 RefractEx(Vec3 incident,
                             Vec3 normal,
                             Float mu,
                             Float cos_i,
                             Float g) MT_NOEXCEPT_RELEASE;
