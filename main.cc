#if defined(XENON_TABLE_COEFFICIENT)
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "base/config/float.h"
#include "modeling/cylinder_plasma_quartz.h"
#include "physics/params/xenon_absorption_coefficient.h"

int main() {
  CylinderPlasmaQuartz::Result rr;

  for (size_t i = 0; i + 1 < kXenonFrequency.size(); ++i) {
    const auto nu_min = kXenonFrequency[i];
    const auto nu_max = kXenonFrequency[i + 1];
    const auto d_nu = nu_max - nu_min;
    const auto nu_avg = nu_min + d_nu / 2;
    std::cout << "[[-----------------------------------------------------------"
                 "-------------------]]\n"
                 "[[i="
              << i << ", nu_min=" << nu_min << ", nu_max=" << nu_max
              << ", d_nu=" << d_nu << ", nu_avg=" << nu_avg
              << "]]\n"
                 "[[-----------------------------------------------------------"
                 "-------------------]]\n";
    auto r = CylinderPlasmaQuartz{nu_avg, d_nu}.Solve();

    Float total_plasma = 0;
    std::cout << "TOTAL ABSORBED PLASMA:\n";
    for (const auto j : r.absorbed_plasma) {
      std::cout << j << '\n';
      total_plasma += j;
    }
    // TODO(a.kerimov): First quartz is the last plasma.
    Float total_quartz = 0;
    std::cout << "TOTAL ABSORBED QUARTZ:\n";
    for (const auto j : r.absorbed_quartz) {
      std::cout << j << '\n';
      total_quartz += j;
    }

    std::cout << "ABSORBED QUARTZ: " << r.absorbed_mirror << '\n';
    std::cout << "SUM: " << total_plasma + total_quartz + r.absorbed_mirror
              << '\n';
    std::cout << "INTENSITY ALL: " << r.intensity_all << '\n';

    if (i == 0) [[unlikely]] {
      rr = std::move(r);
    } else {
      assert(rr.absorbed_plasma.size() == r.absorbed_plasma.size());
      for (size_t j = 0; j < r.absorbed_plasma.size(); ++j) {
        rr.absorbed_plasma[j] += r.absorbed_plasma[j];
      }

      assert(rr.absorbed_quartz.size() == r.absorbed_quartz.size());
      for (size_t j = 0; j < r.absorbed_quartz.size(); ++j) {
        rr.absorbed_quartz[j] += r.absorbed_quartz[j];
      }

      rr.absorbed_mirror += r.absorbed_mirror;
      rr.intensity_all += r.intensity_all;
    }
  }

  Float total_plasma = 0;
  std::cout << "TOTAL ABSORBED PLASMA:\n";
  for (const auto j : rr.absorbed_plasma) {
    std::cout << j << '\n';
    total_plasma += j;
  }
  // TODO(a.kerimov): First quartz is the last plasma.
  Float total_quartz = 0;
  std::cout << "TOTAL ABSORBED QUARTZ:\n";
  for (const auto j : rr.absorbed_quartz) {
    std::cout << j << '\n';
    total_quartz += j;
  }

  std::cout << "ABSORBED QUARTZ: " << rr.absorbed_mirror << '\n';
  std::cout << "SUM: " << total_plasma + total_quartz + rr.absorbed_mirror
            << '\n';
  std::cout << "INTENSITY ALL: " << rr.intensity_all << '\n';
}
#else
#include "base/config/float.h"
#include "modeling/cylinder_plasma_quartz.h"

namespace {
constexpr auto kNu = 1E+15_F;
constexpr auto kDNu = 1E+15_F;
}  // namespace

int main() {
  CylinderPlasmaQuartz{kNu, kDNu}.Solve();
}
#endif
