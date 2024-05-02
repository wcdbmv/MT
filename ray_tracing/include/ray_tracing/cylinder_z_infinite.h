#pragma once

#include "base/config/float.h"
#include "math/linalg/vector.h"
#include "ray_tracing/shape.h"

class CylinderZInfinite final : public Shape {
 public:
  constexpr CylinderZInfinite(Vec3 center, Float radius) noexcept
      : center_{center}, radius2_{radius * radius} {}

  [[nodiscard]] Vec3 Perpendicular(Vec3 p) const noexcept override;
  [[nodiscard]] bool IsOnShape(Vec3 p) const noexcept override;
  [[nodiscard]] Float Intersect(Vec3 pos, Vec3 dir) const noexcept override;

  [[nodiscard]] Float IntersectCurr(Vec3 pos, Vec3 dirs) const noexcept;

  [[nodiscard]] constexpr Vec3 center() const noexcept { return center_; }
  [[nodiscard]] constexpr Float radius2() const noexcept { return radius2_; }

 private:
  Vec3 center_;
  Float radius2_;
};
