#include <iostream>

#include "math/fast_pow.h"
#include "math/linalg/vector.h"
#include "math/linalg/vector_io.h"
#include "physics/consts/planck_constant.h"

int main() {
  std::cout << Vec3{1, Cube(2), consts::kPlanckConstant}.Normalized() << '\n';
}
