#include "math/utils.h"

#include <cassert>

#include "base/float.h"
#include "base/float_cmp.h"

template <std::size_t Size>
std::size_t FindIndexOfMinimalNonNegative(
    const std::array<Float, Size> t) noexcept {
  assert(!t.empty());

  // TODO(a.kerimov): Make more clear.
  auto min = static_cast<Float>(1E+3);
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
Float FindMinimalNonNegative(const std::array<Float, Size> t) noexcept {
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
