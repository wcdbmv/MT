#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/vector3f.h"
#include "ray_tracing/shape.h"

struct Ray;

class Plane : public Shape {
 public:
  constexpr Plane(Vector3F origin, Vector3F normal) noexcept;

  [[nodiscard]] Float Intersect(const Ray& ray) const noexcept override;
  [[nodiscard]] Vector3F Perpendicular(Vector3F p) const noexcept override;
  [[nodiscard]] Vector3F Normal(Vector3F p) const NOEXCEPT_RELEASE override;
  [[nodiscard]] bool IsOnShape(Vector3F p) const noexcept override;

 protected:
  Vector3F origin_;
  Vector3F normal_;
};

constexpr Plane::Plane(const Vector3F origin, const Vector3F normal) noexcept
    : origin_{origin}, normal_{normal} {}
