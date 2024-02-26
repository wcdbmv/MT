#include "math/geometry/shape.h"

#include <cassert>

#include "base/noexcept_release.h"
#include "math/geometry/vector3f.h"

Vector3F Shape::Normal(const Vector3F p) const NOEXCEPT_RELEASE {
  auto normal = NormalUnscaled(p);
  normal.Normalize();
  return normal;
}

Vector3F Shape::ReflectInside(const Vector3F p,
                              const Vector3F dir) const NOEXCEPT_RELEASE {
  const auto normal = -Normal(p);
  return Reflect(normal, dir);
}

Vector3F Shape::ReflectOutside(const Vector3F p,
                               const Vector3F dir) const NOEXCEPT_RELEASE {
  const auto normal = Normal(p);
  return Reflect(normal, dir);
}

Vector3F Shape::Reflect(const Vector3F normal,
                        const Vector3F dir) NOEXCEPT_RELEASE {
  assert(dir.IsNormalized());
  return (dir - 2 * Vector3F::Dot(normal, dir) * normal).Normalized();
}
