#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/vector3f.h"

// TODO(a.kerimov): Move to physics.
[[nodiscard]] Vector3F Reflect(Vector3F I, Vector3F N) NOEXCEPT_RELEASE;
[[nodiscard]] Vector3F ReflectEx(Vector3F I,
                                 Vector3F N,
                                 Float cos_i) NOEXCEPT_RELEASE;
