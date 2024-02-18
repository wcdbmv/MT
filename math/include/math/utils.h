#pragma once

#include <cstddef>
#include <span>

#include "base/float.h"

// TODO(a.kerimov): Move to base?
[[nodiscard]] std::size_t FindMinimalNonNegativeIndex(
    std::span<Float> t) noexcept;
[[nodiscard]] Float FindMinimalNonNegative(std::span<Float> t) noexcept;
