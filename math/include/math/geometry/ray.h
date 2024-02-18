#pragma once

#include "base/float.h"
#include "math/geometry/vector3f.h"

struct Ray {
  Vector3F pos;
  Vector3F dir;

  [[nodiscard]] constexpr Vector3F Point(Float t) const noexcept;
};

constexpr Vector3F Ray::Point(const Float t) const noexcept {
  return pos + dir * t;
}
