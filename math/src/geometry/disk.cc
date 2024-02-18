#include "math/geometry/disk.h"

#include "base/float.h"
#include "math/geometry/plane.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector3f.h"

Float Disk::Intersect(const Ray& ray) const noexcept {
  const auto t = Plane::Intersect(ray);
  if (t < 0) {
    return -1;
  }

  const auto point_on_plane = ray.Point(t);
  const auto distance2_to_origin = (point_on_plane - origin_).SquaredLength();
  if (distance2_to_origin > radius2_) {
    return -1;
  }

  return t;
}

bool Disk::IsOnShape(const Vector3F p) const noexcept {
  if (!Plane::IsOnShape(p)) {
    return false;
  }

  return Vector3F::SquaredDistance(p, origin_) <= radius2_;
}
