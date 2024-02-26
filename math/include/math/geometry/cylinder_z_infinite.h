#pragma once

#include "base/float.h"
#include "math/geometry/ray.h"
#include "math/geometry/shape.h"
#include "math/geometry/vector3f.h"

class CylinderZInfinite : public Shape {
 public:
  constexpr CylinderZInfinite(Vector3F center, Float radius) noexcept;

  [[nodiscard]] Float Intersect(const Ray& ray) const noexcept override;
  [[nodiscard]] Float IntersectCurr(const Ray& ray) const noexcept;
  [[nodiscard]] Vector3F NormalUnscaled(Vector3F p) const noexcept override;

  [[nodiscard]] bool IsOnShape(Vector3F p) const noexcept override;

  [[nodiscard]] constexpr Vector3F center() const noexcept { return center_; }
  [[nodiscard]] constexpr Float radius2() const noexcept { return radius2_; }

  struct FresnelResult {
    Vector3F reflected;
    Vector3F refracted;
    Float R{1};  ///< Коэффициент отражения.
    Float T{0};  ///< Коэффициент преломления.
  };
  [[nodiscard]] FresnelResult Fresnel(Vector3F p,
                                      Vector3F dir,
                                      Float n_1,
                                      Float n_2) const NOEXCEPT_RELEASE;

 protected:
  Vector3F center_;
  Float radius2_;
};

constexpr CylinderZInfinite::CylinderZInfinite(const Vector3F center,
                                               const Float radius) noexcept
    : center_{center}, radius2_{radius * radius} {}
