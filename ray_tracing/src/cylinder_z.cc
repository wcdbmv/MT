#include "ray_tracing/cylinder_z.h"

#include <array>
#include <cassert>
#include <span>

#include "math/linalg/ray.h"
#include "math/utils.h"

Float CylinderZ::Intersect(const Ray& ray) const noexcept {
  std::array<Float, 3> t{-1, -1, -1};
  t[0] = CylinderZInfinite::Intersect(ray);
  if (t[0] >= 0) {
    const auto z0 = ray.pos.z() + ray.dir.z() * t[0];
    if (!IsInsideByZ(z0)) {
      t[0] = -1;
    }
  }

  t[1] = base1_.Intersect(ray);
  t[2] = base2_.Intersect(ray);

  return FindMinimalNonNegative(t);
}

Vector3F CylinderZ::Perpendicular(const Vector3F p) const noexcept {
  if (base1_.IsOnShape(p)) {
    return kUnitZ;
  }

  if (base2_.IsOnShape(p)) {
    return -kUnitZ;
  }

  assert(IsOnSideShape(p));
  return CylinderZInfinite::Perpendicular(p);
}

bool CylinderZ::IsOnShape(const Vector3F p) const noexcept {
  return base1_.IsOnShape(p) || base2_.IsOnShape(p) || IsOnSideShape(p);
}

bool CylinderZ::IsInsideByZ(const Float z) const noexcept {
  return center_.z() <= z && z <= center_.z() + height_;
}

bool CylinderZ::IsOnSideShape(const Vector3F p) const noexcept {
  if (!CylinderZInfinite::IsOnShape(p)) {
    return false;
  }

  return IsInsideByZ(p.z());
}
