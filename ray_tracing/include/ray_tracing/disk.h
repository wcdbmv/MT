#pragma once

#include "base/float.h"
#include "math/linalg/vector3f.h"
#include "ray_tracing/plane.h"

struct Ray;

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
