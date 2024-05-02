#pragma once

#include <algorithm>
#include <cstddef>
#include <experimental/iterator>
#include <ostream>

#include "math/linalg/vector.h"

template <std::size_t Size>
std::ostream& operator<<(std::ostream& os, Vec<Size> v) {
  os << '(';
  std::copy(v.begin(), v.end(),
            std::experimental::make_ostream_joiner(os, ", "));
  os << ')';
  return os;
}
