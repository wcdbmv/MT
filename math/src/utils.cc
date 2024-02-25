#include "math/utils.h"

#include <cstddef>
#include <cassert>
#include <span>

#include "base/float.h"
#include "base/float_cmp.h"

std::size_t FindMinimalNonNegativeIndex(const std::span<Float> t) noexcept {
  assert(!t.empty());

  auto min = kInf;
  auto idx_min = t.size();

  for (size_t i = 0; i < t.size(); ++i) {
    if (kEps <= t[i] && t[i] <= min) {
      min = t[i];
      idx_min = i;
    }
  }

  return idx_min;
}

Float FindMinimalNonNegative(
    const std::span<Float> t) noexcept {
  const auto idx_min = FindMinimalNonNegativeIndex(t);
  if (idx_min != t.size()) {
    return t[idx_min];
  }
  return -1;
}
