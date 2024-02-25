#pragma once

#include <algorithm>
#include <cstddef>
#include <experimental/iterator>
#include <ostream>

#include "math/geometry/vector.h"

template <std::size_t Size, typename T>
std::ostream& operator<<(std::ostream& os, const Vector<Size, T> v) {
  os << '(';
  std::copy(v.begin(), v.end(),
            std::experimental::make_ostream_joiner(os, ", "));
  os << ')';
  return os;
}
