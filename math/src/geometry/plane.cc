#include "math/geometry/plane.h"

#include "base/float.h"
#include "base/float_cmp.h"
#include "base/noexcept_release.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector3f.h"

Float Plane::Intersect(const Ray& ray) const noexcept {
  const auto denominator = Vector3F::Dot(normal_, ray.dir);
  if (denominator < static_cast<Float>(1e-6)) [[unlikely]] {
    return -1;
  }

  return Vector3F::Dot(origin_ - ray.pos, normal_) / denominator;
}

Vector3F Plane::NormalUnscaled(const Vector3F /* p */) const noexcept {
  return normal_;
}

Vector3F Plane::Normal(const Vector3F /* p */) const NOEXCEPT_RELEASE {
  static const auto normal = normal_.Normalized();
  return normal;
}

bool Plane::IsOnShape(const Vector3F p) const noexcept {
  return IsZero(Vector3F::Dot(p - origin_, normal_));
}
