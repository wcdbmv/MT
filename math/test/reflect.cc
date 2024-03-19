#include <gtest/gtest.h>

#include "expect_vector_near.h"
#include "math/linalg/reflect.h"
#include "math/linalg/vector3f.h"

TEST(ReflectTest, ReflectRay45Degree) {
  const auto I = Vector3F{1, -1, 0}.Normalized();
  const auto N = Vector3F{0, -1, 0}.Normalized();
  const auto expected = Vector3F{1, 1, 0}.Normalized();

  const auto R = Reflect(I, N);
  ExpectVectorNear(R, expected);
}

TEST(ReflectTest, ReflectRayFrom90Degree) {
  const auto I = Vector3F{1, -1, 0}.Normalized();
  const auto N = Vector3F{1, -1, 0}.Normalized();
  const auto expected = Vector3F{-1, 1, 0}.Normalized();

  const auto R = Reflect(I, N);
  ExpectVectorNear(R, expected);
}

TEST(ReflectTest, ReflectRayFrom180Degree) {
  const auto I = Vector3F{1, -1, 0}.Normalized();
  const auto N = Vector3F{1, 1, 0}.Normalized();
  const auto expected = Vector3F{1, -1, 0}.Normalized();

  const auto R = Reflect(I, N);
  ExpectVectorNear(R, expected);
}
