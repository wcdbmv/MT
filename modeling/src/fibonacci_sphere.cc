#include "modeling/fibonacci_sphere.h"

#include <cassert>
#include <cmath>

#include "base/config/float.h"
#include "math/consts/golden_ratio.h"
#include "math/consts/pi.h"

namespace {

[[nodiscard]] constexpr Float FindOptimalEpsilon(std::size_t n) noexcept {
  if (n >= 600000) {
    return 214;
  }
  if (n >= 400000) {
    return 75;
  }
  if (n >= 11000) {
    return 27;
  }
  if (n >= 890) {
    return 10;
  }
  if (n >= 177) {
    return 3.33_F;
  }
  if (n >= 24) {
    return 1.33_F;
  }
  return 0.33_F;
}

}  // namespace

std::vector<Vec3> FibonacciSphere(std::size_t n) {
  assert(n > 20);

  const auto epsilon = FindOptimalEpsilon(n);

  std::vector<Vec3> points;
  points.reserve(n);

  const auto nn = static_cast<Float>(n);
  for (std::size_t i = 0; i < n; ++i) {
    const auto ii = static_cast<Float>(i);

    const auto theta = 2 * consts::kPi * ii / consts::kPhi;
    const auto phi = std::acos(1 - 2 * (ii + epsilon) / (nn - 1 + 2 * epsilon));

    const Vec3 p{cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)};

    points.push_back(p);
  }

  return points;
}
