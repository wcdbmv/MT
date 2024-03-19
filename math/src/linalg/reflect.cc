#include "math/linalg/reflect.h"

#include <cassert>

#include "base/float.h"
#include "base/noexcept_release.h"
#include "math/linalg/vector3f.h"

Vector3F Reflect(const Vector3F I, const Vector3F N) NOEXCEPT_RELEASE {
  const auto cos_i = Vector3F::Dot(I, N);
  return ReflectEx(I, N, cos_i);
}

Vector3F ReflectEx(const Vector3F I,
                   const Vector3F N,
                   const Float cos_i) NOEXCEPT_RELEASE {
  assert(I.IsNormalized());
  assert(N.IsNormalized());
  assert(-1 <= cos_i && cos_i <= 1);

  auto R = I - 2 * cos_i * N;
  R.Normalize();

  return R;
}
