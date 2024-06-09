#pragma once

#include <cstddef>
#include <vector>

#include "base/config/float.h"
#include "base/fast_pimpl.h"

struct CylinderPlasma {
  struct Params {
    Float r = 0.35_F;
    std::size_t n_plasma = 40;

    Float t0 = 10000.0_F;
    Float tw = 2000.0_F;
    int m = 4;

    Float rho = 0.95_F;

    Float nu = 1e+15_F;
    Float d_nu = 1e+15_F;

    std::size_t n_meridian = 100;
    std::size_t n_latitude = 100;

    std::size_t n_threads = 4;
  };

  explicit CylinderPlasma(const Params& params);
  ~CylinderPlasma();

  struct Result {
    std::vector<Float> absorbed_plasma;
    std::vector<Float> absorbed_plasma3;
    Float absorbed_mirror{};
    Float intensity_all{};
  };

  Result Solve();

 private:
  class Impl;
  static constexpr std::size_t kSize = 280;
  static constexpr std::size_t kAlignment = 8;
  FastPimpl<Impl, kSize, kAlignment> pimpl_;
};
