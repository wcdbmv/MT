#pragma once

#include <cstddef>
#include <vector>

#include "math/linalg/vector3f.h"

// TODO(a.kerimov): Move to other dir.
// TODO(a.kerimov): Generate points differently.
[[nodiscard]] std::vector<Vector3F> FibonacciSphere(std::size_t n);
