#include "math/geometry/cylinder_z_infinite.h"

#include <array>

#include "base/float.h"
#include "base/float_cmp.h"
#include "math/equation.h"
#include "math/fast_pow.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector3f.h"
#include "math/utils.h"

Float CylinderZInfinite::Intersect(const Ray& ray) const noexcept {
  const auto dx = ray.pos.x() - center_.x();
  const auto dy = ray.pos.y() - center_.y();

  const auto a = Sqr(ray.dir.x()) + Sqr(ray.dir.y());
  const auto b = 2 * (ray.dir.x() * dx + ray.dir.y() * dy);
  const auto c = Sqr(dx) + Sqr(dy) - radius2_;

  std::array<Float, 2> t{};
  const auto res = equation::SolveQuadratic(a, b, c, t[0], t[1]);
  if (res == equation::Result::kNoRealSolution) {
    return -1;
  }
  if (res == equation::Result::kHasInfiniteSolutions) [[unlikely]] {
    return 0;
  }

  return FindMinimalNonNegative(t);
}

Vector3F CylinderZInfinite::NormalUnscaled(Vector3F p) const noexcept {
  return {p.x() - center_.x(), p.y() - center_.y(), 0};
}

bool CylinderZInfinite::IsOnShape(Vector3F p) const noexcept {
  return IsEqual(Sqr(p.x() - center_.x()) + Sqr(p.y() - center_.y()), radius2_);
}
