#include <iostream>

#include "math/fast_pow.h"
#include "math/linalg/vector.h"
#include "math/linalg/vector_io.h"
#include "physics/consts/planck_constant.h"
#include "physics/plancks_law.h"

int main() {
  std::cout << Vec3{func::UNu(1E15, 10000), Cube(2), consts::kPlanckConstant}.Normalized() << '\n';
}
