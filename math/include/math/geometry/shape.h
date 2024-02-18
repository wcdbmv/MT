#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector3f.h"

class Shape {
 public:
  virtual ~Shape() = default;

  [[nodiscard]] virtual Float Intersect(const Ray& ray) const noexcept = 0;

  [[nodiscard]] virtual Vector3F NormalUnscaled(Vector3F p) const noexcept = 0;
  [[nodiscard]] virtual Vector3F Normal(Vector3F p) const NOEXCEPT_RELEASE;
  [[nodiscard]] Vector3F Reflect(Vector3F p,
                                 Vector3F dir) const NOEXCEPT_RELEASE;

  [[nodiscard]] virtual bool IsOnShape(Vector3F p) const noexcept = 0;
};
