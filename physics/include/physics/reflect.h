#pragma once

#include "base/config/float.h"
#include "base/config/noexcept_release.h"
#include "math/linalg/vector.h"

[[nodiscard]] Vec3 Reflect(Vec3 incident, Vec3 normal) MT_NOEXCEPT_RELEASE;
[[nodiscard]] Vec3 ReflectEx(Vec3 incident,
                             Vec3 normal,
                             Float cos_i) MT_NOEXCEPT_RELEASE;
