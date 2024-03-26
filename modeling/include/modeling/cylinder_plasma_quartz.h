#pragma once

#include <cstddef>

#include "base/fast_pimpl.h"

struct CylinderPlasmaQuartz {
  CylinderPlasmaQuartz();
  ~CylinderPlasmaQuartz();

  void Solve();

 private:
  class Impl;
  static constexpr std::size_t kSize = 368;
  static constexpr std::size_t kAlignment = 8;
  FastPimpl<Impl, kSize, kAlignment> pimpl_;
};
