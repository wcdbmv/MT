#include "math/geometry/shape.h"

#include <cassert>

#include "base/noexcept_release.h"
#include "math/geometry/vector3f.h"

Vector3F Shape::Normal(Vector3F p) const NOEXCEPT_RELEASE {
  auto normal = NormalUnscaled(p);
  normal.Normalize();
  return normal;
}

Vector3F Shape::Reflect(Vector3F p, Vector3F dir) const NOEXCEPT_RELEASE {
  const auto normal = -Normal(p);
  assert(dir.IsNormalized());
  return dir - 2 * Vector3F::Dot(normal, dir) * normal;
}
