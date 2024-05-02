#pragma once

#include <cstddef>
#include <vector>

#include "base/config/float.h"
#include "math/linalg/vector.h"
#include "modeling/cylinder_common.h"
#include "modeling/worker.h"
#include "ray_tracing/cylinder_z_infinite.h"

struct HollowCylinder {
  struct Params {
    Vec3 center;
    Float radius_min;
    Float radius_max;
    std::size_t steps;
    Float refractive_index;
    Float refractive_index_internal;
    Float refractive_index_external;
    Float mirror_internal;
    Float mirror_external;
  };

  HollowCylinder(const Params& params,
                 const TemperatureFunc& temperature,
                 const IntensityFunc& intensity,
                 const AttenuationFunc& attenuation);

  [[nodiscard]] WorkerResult SolveDir(const WorkerParams& params) const;

  [[nodiscard]] const Params& params() const { return params_; }

  std::vector<CylinderZInfinite> cylinders;
  std::vector<Float> temperatures;
  std::vector<Float> intensities;
  std::vector<Float> attenuations;

 private:
  Params params_;
};
