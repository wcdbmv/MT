#pragma once

#include "base/config/float.h"

/// @returns Value in range [0, 1].
[[nodiscard]] Float RandFloat() noexcept;

[[nodiscard]] bool ImFeelingLucky(Float probability) noexcept;
