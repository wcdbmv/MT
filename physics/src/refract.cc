#include "physics/refract.h"

#include <cassert>

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/fast_pow.h"
#include "math/linalg/vector3f.h"
#include "math/sqrt.h"

Vector3F Refract(const Vector3F I,
                 const Vector3F N,
                 const Float eta_i,
                 const Float eta_t) NOEXCEPT_RELEASE {
  const auto cos_i = Vector3F::Dot(I, N);
  const auto mu = eta_i / eta_t;
  const auto g2 = 1 - Sqr(mu) * (1 - Sqr(cos_i));
  assert(g2 > 0);
  return RefractEx(I, N, mu, cos_i, Sqrt(g2));
}

Vector3F RefractEx(const Vector3F I,
                   const Vector3F N,
                   const Float mu,
                   const Float cos_i,
                   const Float g) NOEXCEPT_RELEASE {
  assert(I.IsNormalized());
  assert(N.IsNormalized());
  assert(-1 <= cos_i && cos_i <= 1);
  assert(g >= 0);

  // https://physics.stackexchange.com/questions/435512/snells-law-in-vector-form
  auto T = g * N + mu * (I - cos_i * N);
  T.Normalize();

  return T;
}
