#pragma once

#include <cstddef>
#include <vector>

#include "base/config/float.h"
#include "base/fast_pimpl.h"

struct CylinderPlasmaQuartz {
  CylinderPlasmaQuartz(Float nu, Float d_nu);
  ~CylinderPlasmaQuartz();

  struct Result {
    std::vector<Float> absorbed_plasma;
    std::vector<Float> absorbed_quartz;
    Float absorbed_mirror{};
    Float intensity_all{};
  };

  Result Solve();

 private:
  class Impl;
  static constexpr std::size_t kSize = 368;
  static constexpr std::size_t kAlignment = 8;
  FastPimpl<Impl, kSize, kAlignment> pimpl_;
};
