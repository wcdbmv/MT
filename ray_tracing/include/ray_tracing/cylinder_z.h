#pragma once

#include "base/float.h"
#include "math/linalg/vector3f.h"
#include "ray_tracing/cylinder_z_infinite.h"
#include "ray_tracing/disk.h"

struct Ray;

class [[deprecated]] CylinderZ final : public CylinderZInfinite {
 public:
  constexpr CylinderZ(Vector3F center, Float radius, Float height) noexcept;

  [[nodiscard]] Float Intersect(const Ray& ray) const noexcept override;
  [[nodiscard]] Vector3F Perpendicular(Vector3F p) const noexcept override;

  [[nodiscard]] bool IsOnShape(Vector3F p) const noexcept override;

 private:
  [[nodiscard]] bool IsInsideByZ(Float z) const noexcept;
  [[nodiscard]] bool IsOnSideShape(Vector3F p) const noexcept;

  Float height_;

  Disk base1_;
  Disk base2_;
};

constexpr CylinderZ::CylinderZ(const Vector3F center,
                               const Float radius,
                               const Float height) noexcept
    : CylinderZInfinite{center, radius},
      height_{height},
      base1_{center_, kUnitZ, radius},
      base2_{{center_.x(), center_.y(), center_.z() + height},
             -kUnitZ,
             radius} {}
