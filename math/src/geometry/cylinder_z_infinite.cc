#include "math/geometry/cylinder_z_infinite.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>

#include "base/float.h"
#include "base/float_cmp.h"
#include "base/noexcept_release.h"
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

Vector3F CylinderZInfinite::NormalUnscaled(const Vector3F p) const noexcept {
  return {p.x() - center_.x(), p.y() - center_.y(), 0};
}

bool CylinderZInfinite::IsOnShape(const Vector3F p) const noexcept {
  return IsEqual(Sqr(p.x() - center_.x()) + Sqr(p.y() - center_.y()), radius2_);
}

auto CylinderZInfinite::Fresnel(const Vector3F p,
                                const Vector3F dir,
                                const Float n_1,
                                const Float n_2) const
    NOEXCEPT_RELEASE->FresnelResult {
  assert(dir.IsNormalized());

  // TODO(a.kerimov): Fix solution.
  auto n = Normal(p);
  if (n_2 < n_1) {
    n = -n;
  }

  // TODO(a.kerimov): Avoid calc duplication.
  FresnelResult result{.reflected = Reflect(-n, dir)};

  const auto i = dir;
  if (n_1 >= n_2 && Vector3F::Sin(n, i) >= n_2 / n_1) {
    // Полное внутренне отражение.
    return result;
  }

  // https://physics.stackexchange.com/questions/435512/snells-law-in-vector-form
  assert(Vector3F::Cos(n, i) > kEps);
  assert(Vector3F::Sin(n, i) > kEps);
  const auto mu = n_1 / n_2;
  result.refracted =
      std::sqrt(1 - Sqr(mu) * (1 - Sqr(Vector3F::Dot(n, i)))) * n +
      mu * (i - Vector3F::Dot(n, i) * n);
  result.refracted.Normalize();

  // https://steps3d.narod.ru/tutorials/fresnel-tutorial.html
  const auto c = Vector3F::Cos(n, i) * mu;
  assert(c / mu > kEps);
  const auto g = std::sqrt(1 + Sqr(c) - Sqr(mu));

  // Отражённая доля энергии.
  result.R = Sqr((g - c) / (g + c)) *
             (1 + Sqr((c * (g + c) - Sqr(mu)) / (c * (g - c) + Sqr(mu)))) / 2;
  result.T = 1 - result.R;
  assert(0 <= result.R && result.R <= 1);
  assert(0 <= result.T && result.T <= 1);

  return result;
}
