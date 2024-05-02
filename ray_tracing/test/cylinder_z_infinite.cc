#include <gtest/gtest.h>

#include "math/linalg/vector.h"
#include "ray_tracing/cylinder_z_infinite.h"

TEST(CylinderZInfinite, IntersectX) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Vec3 kPos{-10, 0, 0};
  constexpr Vec3 kDir{1, 0, 0};
  ASSERT_EQ(cylinder.Intersect(kPos, kDir), 8);
}

TEST(CylinderZInfinite, IntersectY) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Vec3 kPos{0, -12, 0};
  constexpr Vec3 kDir{0, 1, 0};
  ASSERT_EQ(cylinder.Intersect(kPos, kDir), 10);
}

TEST(CylinderZInfinite, ParallelZ) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Vec3 kPos{0, 0, -14};
  constexpr Vec3 kDir{0, 0, 1};
  ASSERT_EQ(cylinder.Intersect(kPos, kDir), -1);
}

TEST(CylinderZInfinite, OnSideZ) {
  const CylinderZInfinite cylinder{{}, 2};
  constexpr Vec3 kPos{2, 0, -16};
  constexpr Vec3 kDir{0, 0, 1};
  ASSERT_EQ(cylinder.Intersect(kPos, kDir), 0);
}
