#include <gtest/gtest.h>

#include <cmath>

#include "base/config/float.h"
#include "expect_vector_near.h"
#include "math/linalg/vector.h"
#include "physics/refract.h"

TEST(RefractTest, AirToGlass) {
  const auto incident = Vec3{1, 1, -1}.Normalized();
  const auto normal = Vec3{0, 0, -1}.Normalized();
  constexpr auto kEtaI = 1.0_F;
  constexpr auto kEtaT = 1.5_F;
  constexpr auto kMu = kEtaI / kEtaT;
  const auto z = std::sqrt(19) / 3;
  const auto expected = Vec3{kMu, kMu, -z}.Normalized();

  const auto refracted = Refract(incident, normal, kEtaI, kEtaT);
  ExpectVectorNear(refracted, expected);
}

TEST(RefractTest, GlassToAir) {
  const auto expected = Vec3{1, 1, -1}.Normalized();
  const auto normal = Vec3{0, 0, -1}.Normalized();
  constexpr auto kEtaI = 1.0_F;
  constexpr auto kEtaT = 1.5_F;
  constexpr auto kMu = kEtaI / kEtaT;
  const auto z = std::sqrt(19) / 3;
  const auto incident = Vec3{kMu, kMu, -z}.Normalized();

  // NOLINTNEXTLINE(readability-suspicious-call-argument)
  const auto refracted = Refract(-incident, -normal, kEtaT, kEtaI);
  ExpectVectorNear(refracted, -expected);
}
