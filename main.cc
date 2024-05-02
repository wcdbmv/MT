#include <iostream>

#include "math/linalg/vector.h"
#include "math/linalg/vector_io.h"

int main() {
  std::cout << Vec3{1, 2, 5}.Normalized() << '\n';
}
