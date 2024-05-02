#include "ray_tracing/cylinder_z_infinite.h"

#include <algorithm>
#include <array>
#include <cassert>

#include "math/equation.h"
#include "math/fast_pow.h"
#include "math/float/compare.h"
#include "ray_tracing/utils.h"

Vec3 CylinderZInfinite::Perpendicular(Vec3 p) const noexcept {
  return {p.x() - center_.x(), p.y() - center_.y(), 0};
}

bool CylinderZInfinite::IsOnShape(Vec3 p) const noexcept {
  return IsEqual(Sqr(p.x() - center_.x()) + Sqr(p.y() - center_.y()), radius2_);
}

Float CylinderZInfinite::Intersect(Vec3 pos, Vec3 dir) const noexcept {
  const auto dx = pos.x() - center_.x();
  const auto dy = pos.y() - center_.y();

  const auto a = Sqr(dir.x()) + Sqr(dir.y());
  const auto b = 2 * (dir.x() * dx + dir.y() * dy);
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

// TODO(a.kerimov): Refactor.
Float CylinderZInfinite::IntersectCurr(Vec3 pos, Vec3 dir) const noexcept {
  assert(IsOnShape(pos));

  const auto dx = pos.x() - center_.x();
  const auto dy = pos.y() - center_.y();

  const auto a = Sqr(dir.x()) + Sqr(dir.y());
  const auto b = 2 * (dir.x() * dx + dir.y() * dy);
  const auto c = Sqr(dx) + Sqr(dy) - radius2_;

  std::array<Float, 2> t{};
  const auto res = equation::SolveQuadratic(a, b, c, t[0], t[1]);
  if (res == equation::Result::kNoRealSolution) {
    return -1;
  }
  if (res == equation::Result::kHasInfiniteSolutions) [[unlikely]] {
    return 0;
  }

  if (t[0] > 0 && t[1] > 0) {
    return std::max(t[0], t[1]);
  }
  return FindMinimalNonNegative(t);
}
