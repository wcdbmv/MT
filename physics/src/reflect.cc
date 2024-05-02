#include "physics/reflect.h"

#include <cassert>

Vec3 Reflect(Vec3 incident, Vec3 normal) MT_NOEXCEPT_RELEASE {
  const auto cos_i = incident * normal;
  return ReflectEx(incident, normal, cos_i);
}

Vec3 ReflectEx(Vec3 incident, Vec3 normal, Float cos_i) MT_NOEXCEPT_RELEASE {
  assert(incident.IsNormalized());
  assert(normal.IsNormalized());
  assert(-1 <= cos_i && cos_i <= 1);

  auto reflected = incident - 2 * cos_i * normal;
  reflected.Normalize();

  return reflected;
}
