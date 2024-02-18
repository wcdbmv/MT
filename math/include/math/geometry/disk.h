#pragma once

#include "base/float.h"
#include "math/geometry/plane.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector3f.h"

class Disk final : public Plane {
 public:
  constexpr Disk(Vector3F origin, Vector3F normal, Float radius) noexcept;

  [[nodiscard]] Float Intersect(const Ray& ray) const noexcept override;

  [[nodiscard]] bool IsOnShape(Vector3F p) const noexcept override;

 private:
  Float radius2_;
};

constexpr Disk::Disk(const Vector3F origin,
                     const Vector3F normal,
                     const Float radius) noexcept
    : Plane{origin, normal}, radius2_{radius * radius} {}
