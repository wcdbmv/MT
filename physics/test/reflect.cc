#include <gtest/gtest.h>

#include "expect_vector_near.h"
#include "math/linalg/vector.h"
#include "physics/reflect.h"

TEST(ReflectTest, ReflectRay45Degree) {
  const auto incident = Vec3{1, -1, 0}.Normalized();
  const auto normal = Vec3{0, -1, 0}.Normalized();
  const auto expected = Vec3{1, 1, 0}.Normalized();

  const auto reflected = Reflect(incident, normal);
  ExpectVectorNear(reflected, expected);
}

TEST(ReflectTest, ReflectRayFrom90Degree) {
  const auto incident = Vec3{1, -1, 0}.Normalized();
  const auto normal = Vec3{1, -1, 0}.Normalized();
  const auto expected = Vec3{-1, 1, 0}.Normalized();

  const auto reflected = Reflect(incident, normal);
  ExpectVectorNear(reflected, expected);
}

TEST(ReflectTest, ReflectRayFrom180Degree) {
  const auto incident = Vec3{1, -1, 0}.Normalized();
  const auto normal = Vec3{1, 1, 0}.Normalized();
  const auto expected = Vec3{1, -1, 0}.Normalized();

  const auto reflected = Reflect(incident, normal);
  ExpectVectorNear(reflected, expected);
}
