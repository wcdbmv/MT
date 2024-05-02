#pragma once

#include <array>
#include <cstddef>

#include "base/config/float.h"

template <std::size_t Size>
[[nodiscard]] std::size_t FindIndexOfMinimalNonNegative(
    std::array<Float, Size> t) noexcept;

template <std::size_t Size>
[[nodiscard]] Float FindMinimalNonNegative(std::array<Float, Size> t) noexcept;
