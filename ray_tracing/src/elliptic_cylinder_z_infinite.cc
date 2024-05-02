#include "ray_tracing/elliptic_cylinder_z_infinite.h"

#include <algorithm>
#include <array>
#include <cassert>

#include "math/equation.h"
#include "math/fast_pow.h"
#include "math/float/compare.h"
#include "ray_tracing/utils.h"

Vec3 EllipticCylinderZInfinite::Perpendicular(const Vec3 p) const noexcept {
  return {(p.x() - center_.x()) / a2_, (p.y() - center_.y()) / b2_, 0};
}

bool EllipticCylinderZInfinite::IsOnShape(const Vec3 p) const noexcept {
  return IsEqual(
      Sqr(p.x() - center_.x()) / a2_ + Sqr(p.y() - center_.y()) / b2_, kOne);
}

Float EllipticCylinderZInfinite::Intersect(Vec3 pos, Vec3 dir) const noexcept {
  const auto dx = pos.x() - center_.x();
  const auto dy = pos.y() - center_.y();

  const auto a = Sqr(dir.x()) * b2_ + Sqr(dir.y()) * a2_;
  const auto b = 2 * (dir.x() * dx * b2_ + dir.y() * dy * a2_);
  const auto c = Sqr(dx) * b2_ + Sqr(dy) * a2_ - a2_ * b2_;

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
Float EllipticCylinderZInfinite::IntersectCurr(Vec3 pos,
                                               Vec3 dir) const noexcept {
  assert(IsOnShape(pos));

  const auto dx = pos.x() - center_.x();
  const auto dy = pos.y() - center_.y();

  const auto a = Sqr(dir.x()) * b2_ + Sqr(dir.y()) * a2_;
  const auto b = 2 * (dir.x() * dx * b2_ + dir.y() * dy * a2_);
  const auto c = Sqr(dx) * b2_ + Sqr(dy) * a2_ - a2_ * b2_;

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
