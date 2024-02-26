#include <gtest/gtest.h>

#include <cmath>

#include "base/float.h"
#include "expect_vector_near.h"
#include "math/geometry/refract.h"
#include "math/geometry/vector3f.h"

TEST(RefractTest, AirToGlass) {
  const auto I = Vector3F{1, 1, -1}.Normalized();
  const auto N = Vector3F{0, 0, -1}.Normalized();
  constexpr auto kEtaI = static_cast<Float>(1);
  constexpr auto kEtaT = static_cast<Float>(1.5);
  constexpr auto kMu = kEtaI / kEtaT;
  const auto z = std::sqrt(19) / 3;
  const auto expected = Vector3F{kMu, kMu, -z}.Normalized();

  const auto T = Refract(I, N, kEtaI, kEtaT);
  ExpectVectorNear(T, expected);
}

TEST(RefractTest, GlassToAir) {
  const auto expected = Vector3F{1, 1, -1}.Normalized();
  const auto N = Vector3F{0, 0, -1}.Normalized();
  constexpr auto kEtaI = static_cast<Float>(1);
  constexpr auto kEtaT = static_cast<Float>(1.5);
  constexpr auto kMu = kEtaI / kEtaT;
  const auto z = std::sqrt(19) / 3;
  const auto I = Vector3F{kMu, kMu, -z}.Normalized();

  // NOLINTNEXTLINE(readability-suspicious-call-argument)
  const auto T = Refract(-I, -N, kEtaT, kEtaI);
  ExpectVectorNear(T, -expected);
}
