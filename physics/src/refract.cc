#include "physics/refract.h"

#include <cassert>

#include "math/fast_pow.h"
#include "math/float/sqrt.h"

Vec3 Refract(Vec3 incident, Vec3 normal, Float eta_i, Float eta_t)
    MT_NOEXCEPT_RELEASE {
  const auto cos_i = incident * normal;
  const auto mu = eta_i / eta_t;
  const auto g2 = 1 - Sqr(mu) * (1 - Sqr(cos_i));
  assert(g2 > 0);
  return RefractEx(incident, normal, mu, cos_i, Sqrt(g2));
}

Vec3 RefractEx(Vec3 incident, Vec3 normal, Float mu, Float cos_i, Float g)
    MT_NOEXCEPT_RELEASE {
  assert(incident.IsNormalized());
  assert(normal.IsNormalized());
  assert(-1 <= cos_i && cos_i <= 1);
  assert(g >= 0);

  // https://physics.stackexchange.com/questions/435512/snells-law-in-vector-form
  auto refracted = g * normal + mu * (incident - cos_i * normal);
  refracted.Normalize();

  return refracted;
}
