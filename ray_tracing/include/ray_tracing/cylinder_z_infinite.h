#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/ray.h"
#include "math/linalg/vector3f.h"
#include "ray_tracing/shape.h"

class CylinderZInfinite : public Shape {
 public:
  constexpr CylinderZInfinite(Vector3F center, Float radius) noexcept;

  [[nodiscard]] Float Intersect(const Ray& ray) const noexcept override;
  [[nodiscard]] Float IntersectCurr(const Ray& ray) const noexcept;
  [[nodiscard]] Vector3F Perpendicular(Vector3F p) const noexcept override;

  [[nodiscard]] bool IsOnShape(Vector3F p) const noexcept override;

  [[nodiscard]] constexpr Vector3F center() const noexcept { return center_; }
  [[nodiscard]] constexpr Float radius2() const noexcept { return radius2_; }

  struct FresnelResult {
    Vector3F reflected;
    Vector3F refracted;
    Float R{1};  ///< Коэффициент отражения.
    Float T{0};  ///< Коэффициент преломления.
  };
  [[nodiscard]] FresnelResult Refract(const Ray& ray,
                                      Float eta_i,
                                      Float eta_t,
                                      bool outward) const NOEXCEPT_RELEASE;

 protected:
  Vector3F center_;
  Float radius2_;
};

constexpr CylinderZInfinite::CylinderZInfinite(const Vector3F center,
                                               const Float radius) noexcept
    : center_{center}, radius2_{radius * radius} {}
