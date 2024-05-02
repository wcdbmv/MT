#include "ray_tracing/utils.h"

#include <cassert>

#include "base/config/float.h"
#include "math/float/eps.h"

template <std::size_t Size>
std::size_t FindIndexOfMinimalNonNegative(std::array<Float, Size> t) noexcept {
  assert(!t.empty());

  // TODO(a.kerimov): Make more clear.
  auto min = 1E3_F;
  auto idx_min = t.size();

  auto i = static_cast<std::size_t>(0);
  for (const auto f : t) {
    if (kEps <= f && f <= min) {
      min = f;
      idx_min = i;
    }
    ++i;
  }

  return idx_min;
}

template <std::size_t Size>
Float FindMinimalNonNegative(std::array<Float, Size> t) noexcept {
  const auto idx_min = FindIndexOfMinimalNonNegative(t);
  if (idx_min != t.size()) {
    return t[idx_min];
  }
  return -1;
}

template std::size_t FindIndexOfMinimalNonNegative(
    std::array<Float, 2>) noexcept;
template std::size_t FindIndexOfMinimalNonNegative(
    std::array<Float, 3>) noexcept;

template Float FindMinimalNonNegative(std::array<Float, 2>) noexcept;
template Float FindMinimalNonNegative(std::array<Float, 3>) noexcept;
