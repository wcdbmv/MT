#include "expect_vector_near.h"

#include <gtest/gtest.h>

#include "math/float/eps.h"

void ExpectVectorNear(Vec3 actual, Vec3 expected) {
  EXPECT_NEAR(actual.x(), expected.x(), kEps);
  EXPECT_NEAR(actual.y(), expected.y(), kEps);
  EXPECT_NEAR(actual.z(), expected.z(), kEps);
}
