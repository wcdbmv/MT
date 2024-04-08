#pragma once

#include <cstddef>

#include "base/fast_pimpl.h"
#include "base/float.h"

struct CylinderPlasmaQuartz {
  CylinderPlasmaQuartz(Float nu, Float d_nu);
  ~CylinderPlasmaQuartz();

  void Solve();

 private:
  class Impl;
  static constexpr std::size_t kSize = 368;
  static constexpr std::size_t kAlignment = 8;
  FastPimpl<Impl, kSize, kAlignment> pimpl_;
};
