#include <gtest/gtest.h>

#include "base/config/float.h"
#include "math/equation.h"
#include "math/float/eps.h"

TEST(EquationTest, QuadraticEquationTwoRealSolutions) {
  auto x0 = kZero;
  auto x1 = kZero;

  const auto result = equation::SolveQuadratic(5, 6, 1, x0, x1);

  ASSERT_EQ(result, equation::Result::kHasRealSolution);
  ASSERT_NEAR(x0, -1.0_F, kEps);
  ASSERT_NEAR(x1, -0.2_F, kEps);
}

TEST(EquationTest, QuadraticEquationSameRealSolutions) {
  auto x0 = kZero;
  auto x1 = kZero;

  const auto result = equation::SolveQuadratic(1, -4, 4, x0, x1);

  ASSERT_EQ(result, equation::Result::kHasRealSolution);
  ASSERT_NEAR(x0, 2.0_F, kEps);
  ASSERT_NEAR(x1, 2.0_F, kEps);
}

TEST(EquationTest, QuadraticEquationOneRealSolution) {
  auto x0 = kZero;
  auto x1 = kZero;

  const auto result = equation::SolveQuadratic(0, 4, -12, x0, x1);

  ASSERT_EQ(result, equation::Result::kHasRealSolution);
  ASSERT_NEAR(x0, 3.0_F, kEps);
  ASSERT_NEAR(x1, 3.0_F, kEps);
}

TEST(EquationTest, QuadraticEquationNoRealSolutions) {
  auto x0 = kZero;
  auto x1 = kZero;

  const auto result = equation::SolveQuadratic(0, 0, -16, x0, x1);

  ASSERT_EQ(result, equation::Result::kNoRealSolution);
}

TEST(EquationTest, QuadraticEquationInfiniteRealSolutions) {
  auto x0 = kZero;
  auto x1 = kZero;

  const auto result = equation::SolveQuadratic(0, 0, 0, x0, x1);

  ASSERT_EQ(result, equation::Result::kHasInfiniteSolutions);
}
