#pragma once

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/vector3f.h"

struct Ray;

class Shape {
 public:
  virtual ~Shape() = default;

  [[nodiscard]] virtual Float Intersect(const Ray& ray) const noexcept = 0;

  [[nodiscard]] virtual Vector3F Perpendicular(Vector3F p) const noexcept = 0;
  [[nodiscard]] virtual Vector3F Normal(Vector3F p) const NOEXCEPT_RELEASE;

  [[nodiscard]] Vector3F ReflectInside(const Ray& ray) const NOEXCEPT_RELEASE;
  [[nodiscard]] Vector3F ReflectOutside(const Ray& ray) const NOEXCEPT_RELEASE;

  [[nodiscard]] virtual bool IsOnShape(Vector3F p) const noexcept = 0;
};
