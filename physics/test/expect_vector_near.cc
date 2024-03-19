#include "expect_vector_near.h"

#include <gtest/gtest.h>

#include "base/float_cmp.h"
#include "math/linalg/vector3f.h"

void ExpectVectorNear(Vector3F actual, Vector3F expected) {
  EXPECT_NEAR(actual.x(), expected.x(), kEps);
  EXPECT_NEAR(actual.y(), expected.y(), kEps);
  EXPECT_NEAR(actual.z(), expected.z(), kEps);
}
