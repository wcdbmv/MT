#include "ray_tracing/shape.h"

#include <cassert>

#include "math/fast_pow.h"
#include "math/float/sqrt.h"
#include "physics/reflect.h"
#include "physics/refract.h"

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
  } else {
    const auto mu = eta_i / eta_t;
    const auto mu2 = Sqr(mu);

    const auto g2 = 1 - mu2 * (1 - Sqr(cos_i));
    if (g2 <= 0) {
      // Полное внутреннее отражение.
      return result;
    }
    const auto g = Sqrt(g2);

    result.refracted = ::RefractEx(incident, normal, mu, cos_i, g);
    result.refracted.Normalize();

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
