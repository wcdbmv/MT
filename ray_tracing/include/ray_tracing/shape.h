#pragma once

#include "base/config/float.h"
#include "base/config/noexcept_release.h"
#include "math/linalg/vector.h"

class Shape {
 public:
  virtual ~Shape() = default;

  [[nodiscard]] virtual Vec3 Perpendicular(Vec3 p) const noexcept = 0;
  [[nodiscard]] virtual bool IsOnShape(Vec3 p) const noexcept = 0;
  [[nodiscard]] virtual Float Intersect(Vec3 pos, Vec3 dir) const noexcept = 0;

  [[nodiscard]] virtual Vec3 Normal(Vec3 p) const MT_NOEXCEPT_RELEASE;

  [[nodiscard]] Vec3 ReflectInside(Vec3 pos,
                                   Vec3 dir) const MT_NOEXCEPT_RELEASE;
  [[nodiscard]] Vec3 ReflectOutside(Vec3 pos,
                                    Vec3 dir) const MT_NOEXCEPT_RELEASE;

  struct FresnelResult {
    Vec3 reflected;
    Vec3 refracted;
    Float R{1};  ///< Коэффициент отражения.
    Float T{0};  ///< Коэффициент преломления.
  };
  [[nodiscard]] FresnelResult Refract(Vec3 pos,
                                      Vec3 dir,
                                      Float eta_i,
                                      Float eta_t,
                                      Float mirror,
                                      bool outward) const MT_NOEXCEPT_RELEASE;
};
