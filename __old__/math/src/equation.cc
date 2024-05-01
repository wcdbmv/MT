#include "math/equation.h"

#include "base/float_cmp.h"
#include "math/fast_pow.h"
#include "math/sqrt.h"

namespace equation {

Result SolveLinear(const Float a, const Float b, Float& x) noexcept {
  if (IsZero(a)) [[unlikely]] {
    if (IsZero(b)) [[unlikely]] {
      return Result::kHasInfiniteSolutions;
    }
    return Result::kNoRealSolution;
  }

  x = -b / a;
  return Result::kHasRealSolution;
}

Result SolveQuadratic(const Float a,
                      const Float b,
                      const Float c,
                      Float& x0,
                      Float& x1) noexcept {
  if (IsZero(a)) [[unlikely]] {
    const auto result = SolveLinear(b, c, x0);
    x1 = x0;
    return result;
  }

  const auto _2a = 2 * a;
  const auto discriminant = Sqr(b) - 2 * _2a * c;
  if (discriminant < 0) {
    return Result::kNoRealSolution;
  }

  const auto sqrt_d = Sqrt(discriminant);
  x0 = (-b - sqrt_d) / _2a;
  x1 = (-b + sqrt_d) / _2a;
  return Result::kHasRealSolution;
}

}  // namespace equation
