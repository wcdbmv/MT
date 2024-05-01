#pragma once

#include <vector>

#include "base/float.h"
#include "math/linalg/ray.h"

struct WorkerParams {
  Ray ray;
  Float intensity{};
  Float intensity_end{};
  bool use_prev{false};
};

struct WorkerResult {
  std::vector<WorkerParams> released_rays;
  std::vector<Float> absorbed;
  Float absorbed_at_the_border{};
};
