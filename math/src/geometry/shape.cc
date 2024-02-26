#include "math/geometry/shape.h"

#include "base/noexcept_release.h"
#include "math/geometry/ray.h"
#include "math/geometry/reflect.h"
#include "math/geometry/vector3f.h"

Vector3F Shape::Normal(const Vector3F p) const NOEXCEPT_RELEASE {
  return Perpendicular(p).Normalized();
}

Vector3F Shape::ReflectInside(const Ray& ray) const NOEXCEPT_RELEASE {
  const auto N = -Normal(ray.pos);
  return Reflect(ray.dir, N);
}

Vector3F Shape::ReflectOutside(const Ray& ray) const NOEXCEPT_RELEASE {
  const auto N = Normal(ray.pos);
  return Reflect(ray.dir, N);
}
