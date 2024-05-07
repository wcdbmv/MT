#include "physics/params/plasma.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iterator>

#include "base/ignore_unused.h"
#include "math/fast_pow.h"
#include "math/float/compare.h"
#include "math/float/exp.h"
#include "math/float/log.h"
#include "physics/params/xenon_absorption_coefficient.h"

namespace {

MT_IGNORE_UNUSED_BEGIN

[[nodiscard]] constexpr Float AbsortionCoefficientFromConstant(
    Float nu,
    Float t) noexcept {
  IgnoreUnused(nu);
  IgnoreUnused(t);
  return 10000.0_F;
}

[[nodiscard]] constexpr Float AbsortionCoefficientFromDefault(
    Float nu,
    Float t) noexcept {
  IgnoreUnused(nu);
  return 0.04_F * Sqr(t / 2000);
}

MT_IGNORE_UNUSED_END

}  // namespace

namespace params::plasma {

Float AbsortionCoefficientFromTable(Float nu, Float t) noexcept {
  static_assert(kXenonTemperature.size() == 13);
  assert(IsEqual(kXenonTemperature[0], 2000.0_F));
  assert(IsEqual(kXenonTemperature[1], 3000.0_F));
  assert(IsEqual(kXenonTemperature[2], 4000.0_F));
  assert(IsEqual(kXenonTemperature[3], 5000.0_F));
  assert(IsEqual(kXenonTemperature[4], 6000.0_F));
  assert(IsEqual(kXenonTemperature[5], 7000.0_F));
  assert(IsEqual(kXenonTemperature[6], 8000.0_F));
  assert(IsEqual(kXenonTemperature[7], 9000.0_F));
  assert(IsEqual(kXenonTemperature[8], 10000.0_F));
  assert(IsEqual(kXenonTemperature[9], 11000.0_F));
  assert(IsEqual(kXenonTemperature[10], 12000.0_F));
  assert(IsEqual(kXenonTemperature[11], 13000.0_F));
  assert(IsEqual(kXenonTemperature[12], 14000.0_F));

  assert(kXenonTemperature.front() <= t && t <= kXenonTemperature.back());
  const auto t_idx = static_cast<size_t>(t) / 1000 - 2;
  assert(t_idx <= 10);

  const auto* lower = std::ranges::lower_bound(kXenonFrequency, nu);
  assert(lower != kXenonFrequency.end());
  const auto nu_idx_p = std::distance(kXenonFrequency.begin(), lower);
  assert(1 <= nu_idx_p);
  const auto nu_idx = static_cast<size_t>(nu_idx_p - 1);
  assert(nu_idx + 1 < kXenonFrequency.size());

  // TODO(a.kerimov): Cache?
  const auto t_ln = Log(t);
  const auto t0_ln = Log(kXenonTemperature[t_idx]);
  const auto t1_ln = Log(kXenonTemperature[t_idx + 1]);

  const auto f0 = kXenonAbsorptionCoefficient[t_idx][nu_idx];
  const auto f1 = kXenonAbsorptionCoefficient[t_idx + 1][nu_idx];
  const auto f0_ln = Log(f0);
  const auto f1_ln = Log(f1);

  const auto f_ln = f0_ln + (t_ln - t0_ln) * (f1_ln - f0_ln) / (t1_ln - t0_ln);
  const auto f = Exp(f_ln);
  return f;
}

Float AbsortionCoefficient(Float nu, Float t) noexcept {
#if defined(CONSTANT_TEMPERATURE)
  return AbsortionCoefficientFromConstant(nu, t);
#elif defined(XENON_TABLE_COEFFICIENT)
  return AbsortionCoefficientFromTable(nu, t);
#else
  return AbsortionCoefficientFromDefault(nu, t);
#endif
}

}  // namespace params::plasma
