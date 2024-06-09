#include "ray_tracing/shape.h"

#include <cassert>
#include <cmath>

#include "math/fast_pow.h"
#include "physics/reflect.h"
#include "physics/refract.h"

#ifdef MT_USE_DIFFUSE_REFLECTION
#include "math/random.h"
#endif

Vec3 Shape::Normal(Vec3 p) const MT_NOEXCEPT_RELEASE {
  return Perpendicular(p).Normalized();
}

Vec3 Shape::ReflectInside(Vec3 pos, Vec3 dir) const MT_NOEXCEPT_RELEASE {
  const auto n = -Normal(pos);
  return Reflect(dir, n);
}

Vec3 Shape::ReflectOutside(Vec3 pos, Vec3 dir) const MT_NOEXCEPT_RELEASE {
  const auto n = Normal(pos);
  return Reflect(dir, n);
}

Shape::FresnelResult Shape::Refract(Vec3 pos,
                                    Vec3 dir,
                                    Float eta_i,
                                    Float eta_t,
                                    Float mirror,
                                    bool outward) const MT_NOEXCEPT_RELEASE {
  const auto incident = dir;
  assert(incident.IsNormalized());

  auto normal = Normal(pos);
  if (!outward) {
    normal.Negate();
  }

  const auto cos_i = incident * normal;
  assert(cos_i > 0);

  FresnelResult result{.reflected = ReflectEx(incident, -normal, -cos_i),
                       .refracted = incident};

  if (mirror > 0) {
    result.R = mirror;

#ifdef MT_USE_DIFFUSE_REFLECTION
    assert(outward);
    const auto i = 2 * RandFloat() - 1;
    const auto j_max = std::sqrt(1 - Sqr(i));
    const auto j = 2 * j_max * (RandFloat() - 0.5_F);
    const auto k = std::sqrt(1 - Sqr(i) - Sqr(j));

    const auto n1 = Vec3::Cross(incident, -normal).Normalized();
    const auto n2 = Vec3::Cross(-normal, n1).Normalized();

    result.reflected = i * n1 + j * n2 - k * normal;
    result.reflected.Normalize();
#endif  // MT_USE_DIFFUSE_REFLECTION
  } else {
    const auto mu = eta_i / eta_t;
    const auto mu2 = Sqr(mu);

    const auto g2 = 1 - mu2 * (1 - Sqr(cos_i));
    if (g2 <= 0) {
      // Полное внутреннее отражение.
      return result;
    }
    const auto g = std::sqrt(g2);

    result.refracted = ::RefractEx(incident, normal, mu, cos_i, g);

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
