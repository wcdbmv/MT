#pragma once

#include <cstdint>
#include <utility>

#include "base/float.h"

namespace equation {

enum class Result : std::uint8_t {
  kNoRealSolution,
  kHasRealSolution,
  kHasInfiniteSolutions,
};

[[nodiscard]] Result SolveLinear(Float a, Float b, Float& x) noexcept;
[[nodiscard]] Result SolveQuadratic(Float a,
                                    Float b,
                                    Float c,
                                    Float& x0,
                                    Float& x1) noexcept;

}  // namespace equation
