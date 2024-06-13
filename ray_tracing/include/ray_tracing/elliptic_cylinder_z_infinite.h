#pragma once

#include "base/config/float.h"
#include "math/linalg/vector.h"
#include "ray_tracing/shape.h"

class EllipticCylinderZInfinite final : public Shape {
 public:
  // 2a = width, 2b = height.
  constexpr EllipticCylinderZInfinite(Vec3 center, Float a, Float b) noexcept
      : center_{center}, a2_{a * a}, b2_{b * b} {}

  [[nodiscard]] Vec3 Perpendicular(Vec3 p) const noexcept override;
  [[nodiscard]] bool IsOnShape(Vec3 p) const noexcept override;
  [[nodiscard]] Float Intersect(Vec3 pos, Vec3 dir) const noexcept override;

  [[nodiscard]] Float IntersectCurr(Vec3 pos, Vec3 dir) const noexcept;

  [[nodiscard]] constexpr Vec3 center() const noexcept { return center_; }
  [[nodiscard]] constexpr Float a2() const noexcept { return a2_; }
  [[nodiscard]] constexpr Float b2() const noexcept { return b2_; }

 private:
  Vec3 center_;
  Float a2_;
  Float b2_;
};
