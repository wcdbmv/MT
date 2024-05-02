#pragma once

#include <vector>

#include "base/config/float.h"
#include "math/linalg/vector.h"

struct WorkerParams {
  Vec3 pos;
  Vec3 dir;
  Float intensity{};
  Float intensity_end{};
  bool use_prev{false};
};

struct WorkerResult {
  std::vector<WorkerParams> released_rays;
  std::vector<Float> absorbed;
  Float absorbed_at_the_border{};
};
