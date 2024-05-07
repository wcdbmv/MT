#pragma once

#include <cstddef>
#include <vector>

#include "base/config/float.h"
#include "base/fast_pimpl.h"

struct CylinderPlasma {
  CylinderPlasma(Float nu, Float d_nu);
  ~CylinderPlasma();

  struct Result {
    std::vector<Float> absorbed_plasma;
    Float absorbed_mirror{};
    Float intensity_all{};
  };

  Result Solve();

 private:
  class Impl;
  static constexpr std::size_t kSize = 184;
  static constexpr std::size_t kAlignment = 8;
  FastPimpl<Impl, kSize, kAlignment> pimpl_;
};
