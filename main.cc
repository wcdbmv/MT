#include <iostream>

#include "math/float/pow.h"
#include "math/linalg/vector.h"
#include "math/linalg/vector_io.h"

int main() {
  std::cout << Vec3{1, Cube(2), 5}.Normalized() << '\n';
}
