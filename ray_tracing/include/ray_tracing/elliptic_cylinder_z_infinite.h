#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/vector3f.h"
#include "ray_tracing/shape.h"

struct Ray;

class EllipticCylinderZInfinite : public Shape {
 public:
  // 2a = width, 2b = height.
  constexpr EllipticCylinderZInfinite(Vector3F center,
                                      Float a,
                                      Float b) noexcept;

  [[nodiscard]] Float Intersect(const Ray& ray) const noexcept override;
  [[nodiscard]] Float IntersectCurr(const Ray& ray) const noexcept;
  [[nodiscard]] Vector3F Perpendicular(Vector3F p) const noexcept override;

  [[nodiscard]] bool IsOnShape(Vector3F p) const noexcept override;

  [[nodiscard]] constexpr Vector3F center() const noexcept { return center_; }

  struct FresnelResult {
    Vector3F reflected;
    Vector3F refracted;
    Float R{1};  ///< Коэффициент отражения.
    Float T{0};  ///< Коэффициент преломления.
  };
  [[nodiscard]] FresnelResult Refract(const Ray& ray,
                                      Float eta_i,
                                      Float eta_t,
                                      Float mirror,
                                      bool outward) const NOEXCEPT_RELEASE;

 protected:
  Vector3F center_;
  Float a2_;
  Float b2_;
};

constexpr EllipticCylinderZInfinite::EllipticCylinderZInfinite(
    const Vector3F center,
    const Float a,
    const Float b) noexcept
    : center_{center}, a2_{a * a}, b2_{b * b} {}
