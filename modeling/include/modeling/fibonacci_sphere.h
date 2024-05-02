#pragma once

#include <cstddef>
#include <vector>

#include "math/linalg/vector.h"

// TODO(a.kerimov): Generate points differently.
[[nodiscard]] std::vector<Vec3> FibonacciSphere(std::size_t n);
