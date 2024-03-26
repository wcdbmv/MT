#include "ray_tracing/elliptic_cylinder_z_infinite.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <span>

#include "base/float_cmp.h"
#include "math/equation.h"
#include "math/fast_pow.h"
#include "math/linalg/ray.h"
#include "math/sqrt.h"
#include "math/utils.h"
#include "physics/reflect.h"
#include "physics/refract.h"

Float EllipticCylinderZInfinite::Intersect(const Ray& ray) const noexcept {
  const auto dx = ray.pos.x() - center_.x();
  const auto dy = ray.pos.y() - center_.y();

  const auto a = Sqr(ray.dir.x()) * b2_ + Sqr(ray.dir.y()) * a2_;
  const auto b = 2 * (ray.dir.x() * dx * b2_ + ray.dir.y() * dy * a2_);
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
Float EllipticCylinderZInfinite::IntersectCurr(const Ray& ray) const noexcept {
  assert(IsOnShape(ray.pos));

  const auto dx = ray.pos.x() - center_.x();
  const auto dy = ray.pos.y() - center_.y();

  const auto a = Sqr(ray.dir.x()) * b2_ + Sqr(ray.dir.y()) * a2_;
  const auto b = 2 * (ray.dir.x() * dx * b2_ + ray.dir.y() * dy * a2_);
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

Vector3F EllipticCylinderZInfinite::Perpendicular(
    const Vector3F p) const noexcept {
  return {(p.x() - center_.x()) / a2_, (p.y() - center_.y()) / b2_, 0};
}

bool EllipticCylinderZInfinite::IsOnShape(const Vector3F p) const noexcept {
  return IsEqual(
      Sqr(p.x() - center_.x()) / a2_ + Sqr(p.y() - center_.y()) / b2_,
      static_cast<Float>(1));
}

// TODO(a.kerimov): Refactor.
EllipticCylinderZInfinite::FresnelResult EllipticCylinderZInfinite::Refract(
    const Ray& ray,
    const Float eta_i,
    const Float eta_t,
    const Float mirror,
    const bool outward) const NOEXCEPT_RELEASE {
  const auto I = ray.dir;
  assert(I.IsNormalized());

  auto N = Normal(ray.pos);
  if (!outward) {
    N.Negate();
  }

  const auto cos_i = Vector3F::Dot(I, N);
  assert(cos_i > 0);

  FresnelResult result{.reflected = ReflectEx(I, -N, -cos_i)};

  const auto mu = eta_i / eta_t;
  const auto mu2 = Sqr(mu);

  const auto g2 = 1 - mu2 * (1 - Sqr(cos_i));
  if (g2 <= 0) {
    // Полное внутреннее отражение.
    return result;
  }
  const auto g = Sqrt(g2);

  result.refracted = ::RefractEx(I, N, mu, cos_i, g);
  result.refracted.Normalize();

  if (mirror > 0) {
    result.R = mirror;
  } else {
    // https://steps3d.narod.ru/tutorials/fresnel-tutorial.html
    const auto c = cos_i * mu;

    // Отражённая доля энергии.
    result.R = Sqr((g - c) / (g + c)) *
               (1 + Sqr((c * (g + c) - mu2) / (c * (g - c) + mu2))) / 2;
  }

  result.T = 1 - result.R;
  assert(0 <= result.R && result.R <= 1);
  assert(0 <= result.T && result.T <= 1);

  return result;
}
