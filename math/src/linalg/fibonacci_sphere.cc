#include "math/linalg/fibonacci_sphere.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <vector>

#include "base/float.h"
#include "math/consts/golden_ratio.h"
#include "math/consts/pi.h"
#include "math/linalg/vector3f.h"

namespace {

[[nodiscard]] constexpr Float FindOptimalEpsilon(const std::size_t n) noexcept {
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
    return static_cast<Float>(3.33);
  }
  if (n >= 24) {
    return static_cast<Float>(1.33);
  }
  return static_cast<Float>(0.33);
}

}  // namespace

std::vector<Vector3F> FibonacciSphere(const std::size_t n) {
  assert(n > 20);

  const auto epsilon = FindOptimalEpsilon(n);

  std::vector<Vector3F> points;
  points.reserve(n);

  const auto nn = static_cast<Float>(n);
  for (std::size_t i = 0; i < n; ++i) {
    const auto ii = static_cast<Float>(i);

    const auto theta = 2 * consts::pi * ii / consts::phi;
    const auto phi = std::acos(1 - 2 * (ii + epsilon) / (nn - 1 + 2 * epsilon));

    const Vector3F p{cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)};

    points.push_back(p);
  }

  return points;
}
