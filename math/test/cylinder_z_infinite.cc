#include <gtest/gtest.h>

#include "math/geometry/cylinder_z_infinite.h"
#include "math/geometry/ray.h"

TEST(CylinderZInfinite, IntersectX) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Ray ray{{-10, 0, 0}, {1, 0, 0}};
  ASSERT_EQ(cylinder.Intersect(ray), 8);
}

TEST(CylinderZInfinite, IntersectY) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Ray ray{{0, -12, 0}, {0, 1, 0}};
  ASSERT_EQ(cylinder.Intersect(ray), 10);
}

TEST(CylinderZInfinite, ParallelZ) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Ray ray{{0, 0, -14}, {0, 0, 1}};
  ASSERT_EQ(cylinder.Intersect(ray), -1);
}

TEST(CylinderZInfinite, OnSideZ) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Ray ray{{2, 0, -16}, {0, 0, 1}};
  ASSERT_EQ(cylinder.Intersect(ray), 0);
}
