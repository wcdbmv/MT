#pragma once

#include <cstddef>
#include <vector>

#include "base/config/float.h"
#include "base/fast_pimpl.h"

#include "physics/params/plasma.h"
#include "physics/params/quartz.h"

struct CylinderPlasmaQuartz {
  struct Params {
    Float r = 0.35_F;
    std::size_t n_plasma = 40;

    Float delta = 0.1_F;
    std::size_t n_quartz = 15;

    Float t0 = 10000.0_F;
    Float tw = 2000.0_F;
    int m = 4;
    Float t1 = 700.0_F;

    Float eta_plasma = params::plasma::kEta;
    Float eta_quartz = params::quartz::kEta;
    Float rho = 0.95_F;

    Float nu = 1e+15_F;
    Float d_nu = 1e+15_F;

    std::size_t n_meridian = 100;
    std::size_t n_latitude = 100;

    std::size_t n_threads = 4;
    Float i_crit = 0.000001_F;
  };

  CylinderPlasmaQuartz(const Params& params);
  ~CylinderPlasmaQuartz();

  struct Result {
    std::vector<Float> absorbed_plasma;
    std::vector<Float> absorbed_plasma3;
    std::vector<Float> absorbed_quartz;
    std::vector<Float> absorbed_quartz3;
    Float absorbed_mirror{};
    Float intensity_all{};
  };

  Result Solve();

 private:
  class Impl;
  static constexpr std::size_t kSize = 528;
  static constexpr std::size_t kAlignment = 8;
  FastPimpl<Impl, kSize, kAlignment> pimpl_;
};
