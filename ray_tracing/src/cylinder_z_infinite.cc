#include "ray_tracing/cylinder_z_infinite.h"

#include <algorithm>
#include <array>
#include <cassert>

#include "base/float.h"
#include "base/float_cmp.h"
#include "base/noexcept_release.h"
#include "math/equation.h"
#include "math/fast_pow.h"
#include "math/linalg/ray.h"
#include "math/linalg/vector3f.h"
#include "math/sqrt.h"
#include "math/utils.h"
#include "physics/reflect.h"
#include "physics/refract.h"

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

Float CylinderZInfinite::IntersectCurr(const Ray& ray) const noexcept {
  assert(IsOnShape(ray.pos));

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

  if (t[0] > 0 && t[1] > 0) {
    return std::max(t[0], t[1]);
  }
  return FindMinimalNonNegative(t);
}

Vector3F CylinderZInfinite::Perpendicular(const Vector3F p) const noexcept {
  return {p.x() - center_.x(), p.y() - center_.y(), 0};
}

bool CylinderZInfinite::IsOnShape(const Vector3F p) const noexcept {
  return IsEqual(Sqr(p.x() - center_.x()) + Sqr(p.y() - center_.y()), radius2_);
}

CylinderZInfinite::FresnelResult CylinderZInfinite::Refract(
    const Ray& ray,
    const Float eta_i,
    const Float eta_t,
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

  // https://steps3d.narod.ru/tutorials/fresnel-tutorial.html
  const auto c = cos_i * mu;

  // Отражённая доля энергии.
  result.R = Sqr((g - c) / (g + c)) *
             (1 + Sqr((c * (g + c) - mu2) / (c * (g - c) + mu2))) / 2;
  result.T = 1 - result.R;
  assert(0 <= result.R && result.R <= 1);
  assert(0 <= result.T && result.T <= 1);

  return result;
}
