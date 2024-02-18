#include "base/float_cmp.h"
#include "math/consts/pi.h"
namespace {

// using Float = float;
//
// // u_nu_e — функция Планка.
// constexpr Float Calculate_u_nu_e(const Float nu, const Float T) {
//   return 8 * pi * h * Cube(nu) / (Cube(c) * (std::exp(h * nu / (k * T)) -
//   1));
// }
//
// constexpr Float CalculateDivVecF_l(
//     const Float k_nu,
//     const Float u_nu_e,  // e — equilibrium, равновесная
//     const Float u) {
//   return c * k_nu * (u_nu_e - u);
// }
//
// constexpr Float CalculateDeltaOmega(const Float DeltaS, const Float r) {
//   return DeltaS / (r * r);
// }
//
// constexpr Float CalculateDeltaE(
//     const Float k_nu,
//     const Float u_nu_e,  // e — equilibrium, равновесная
//     const Float DeltaOmega,
//     const Float DeltaV,
//     const Float N_f) {
//   return c * k_nu * u_nu_e * DeltaOmega * DeltaV / (4 * pi * N_f);
// }

}  // namespace

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>
#include "base/float.h"
#include "math/geometry/cylinder_z_infinite.h"
#include "math/geometry/disk.h"
#include "math/geometry/fibonacci_sphere.h"
#include "math/geometry/plane.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector.h"
#include "math/geometry/vector3f.h"
#include "math/geometry/vector_io.h"
#include "math/utils.h"
#include "physics/params.h"

namespace {

constexpr auto kStep = static_cast<Float>(0.05);
const auto kN = static_cast<size_t>(std::round(params::R_1 / kStep));
const auto kPlasmaIdx = static_cast<size_t>(std::round(params::R / kStep) - 1);

class CylinderPlasmaQuartz {
 public:
  CylinderPlasmaQuartz() {
    InitCylinders();
    InitMirrors();
    InitBorders();
    InitDirs();

    assert(kN == 9);
  }

  void Solve() { SolveDir(dirs_[0]); }

 private:
  void InitCylinders() {
    cylinders_.reserve(kN);
    temperatures_.reserve(kN);

    for (size_t i = 1; i <= kPlasmaIdx; ++i) {
      const auto radius = kStep * static_cast<Float>(i);
      cylinders_.emplace_back(kOrigin, radius);
      const auto center =
          kStep * (static_cast<Float>(i) - static_cast<Float>(0.5));
      temperatures_.emplace_back(params::T(center / params::R));
    }

    cylinders_.emplace_back(kOrigin, params::R);
    assert(cylinders_.size() == kPlasmaIdx + 1);
    temperatures_.emplace_back(params::T((params::R - kStep / 2) / params::R));
    assert(temperatures_.size() == kPlasmaIdx + 1);

    for (size_t i = kPlasmaIdx + 2; i < kN; ++i) {
      const auto radius = kStep * static_cast<Float>(i);
      cylinders_.emplace_back(kOrigin, radius);
      const auto center =
          kStep * (static_cast<Float>(i) - static_cast<Float>(0.5));
      temperatures_.emplace_back(params::T(center / params::R));
    }

    cylinders_.emplace_back(kOrigin, params::R_1);
    temperatures_.emplace_back(
        params::T((params::R_1 - kStep / 2) / params::R));

    std::cout << "Cylinders:\n";
    for (auto& cylinder : cylinders_) {
      std::cout << cylinder.center() << ' ' << std::sqrt(cylinder.radius2())
                << '\n';
    }

    std::cout << "T:\n";
    for (auto t : temperatures_) {
      std::cout << t << '\n';
    }
  }

  void InitMirrors() {
    constexpr auto kAmountOfSlices = 4;
    constexpr auto kAngleOfSlice = consts::pi / kAmountOfSlices;
    const auto kCos = std::cos(kAngleOfSlice);
    const auto kSin = std::sin(kAngleOfSlice);
    const auto kNormal1 = Vector3F{-kSin, kCos};
    const auto kNormal2 = Vector3F{-kSin, -kCos};

    mirrors_.reserve(2);
    mirrors_.emplace_back(kOrigin, kNormal1);
    mirrors_.emplace_back(kOrigin, kNormal2);
  }

  void InitBorders() {
    borders_.reserve(2);
    borders_.emplace_back(kOrigin, -kUnitZ, params::R_1);
    borders_.emplace_back(kOrigin + Vector3F{0, 0, params::H}, kUnitZ,
                          params::R_1);
  }

  void InitDirs() {
    dirs_ = FibonacciSphere(100);
    for (auto dir : dirs_) {
      std::cout << dir << '\n';
    }
  }

  void SolveDir(const Vector3F dir) {
    size_t current_cylinder_idx = kPlasmaIdx;

    constexpr auto kInitialPos = Vector3F{params::R, 0, params::H / 2};
    Ray ray{kInitialPos, dir};

    for (size_t i = 0; i < 20; ++i) {
      std::vector<Float> ts;
      if (current_cylinder_idx > 0) {
        const auto t = cylinders_[current_cylinder_idx - 1].Intersect(ray);
        std::cout << '[' << i << "] " << t << ' ' << ray.Point(t) << '\n';
        ts.push_back(t);
      } else {
        ts.push_back(-1);
      }
      if (current_cylinder_idx + 1 < cylinders_.size()) {
        const auto t = cylinders_[current_cylinder_idx + 1].Intersect(ray);
        std::cout << '[' << i << "] " << t << ' ' << ray.Point(t) << '\n';
        ts.push_back(t);
      } else {
        ts.push_back(-1);
      }
      for (auto& mirror : mirrors_) {
        const auto t = mirror.Intersect(ray);
        std::cout << '[' << i << "] " << t << ' ' << ray.Point(t) << '\n';
        ts.push_back(t);
      }

      const auto t_min_idx = FindMinimalNonNegativeIndex(ts);
      if (t_min_idx < 2) {
        ray.pos = ray.Point(ts[t_min_idx]);
        if (t_min_idx == 0) {
          --current_cylinder_idx;
        } else {
          ++current_cylinder_idx;
        }
        std::cout << "NEW POS: " << ray.pos << " [ti=" << t_min_idx
                  << "][ci=" << current_cylinder_idx << "]\n";

        if (current_cylinder_idx + 1 == cylinders_.size()) {
          ray.dir = cylinders_.back().Reflect(ray.pos, ray.dir);
          std::cout << "REFLECT QUARTZ, new dir " << ray.dir << '\n';
        }
      } else if (t_min_idx < 4) {
        if (IsEqual(ts[2], ts[3])) [[unlikely]] {
          ray.pos.x() = 0;
          ray.pos.y() = 0;
          ray.dir.x() = -ray.dir.x();
          std::cout << "REFLECT ORIGIN, new dir" << ray.dir << '\n';
        } else {
          ray.dir = mirrors_[t_min_idx - 2].Reflect(ray.pos, ray.dir);
          std::cout << "REFLECT MIRROR, new dir" << ray.dir << '\n';
        }
        current_cylinder_idx = 1;
      } else {
        [[maybe_unused]] auto x = 12;
      }
    }
  }

  std::vector<CylinderZInfinite> cylinders_;
  std::vector<Float> temperatures_;
  std::vector<Plane> mirrors_;
  std::vector<Disk> borders_;

  std::vector<Vector3F> dirs_;
};

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
  std::cout << params::T(0) << '\n';
  std::cout << params::T(1) << '\n';
  std::cout << params::T(params::R_1 / params::R) << '\n';

  std::cout << Vector<2, Float>{-1}.x() << '\n';
  std::cout << Vector<2, Float>{1}.y() << '\n';

  const CylinderZInfinite c{Vector3F{}, 1};
  std::cout << c.Intersect({{0, 0.5, 0}, {1, 0, 0}}) << '\n';
  std::cout << c.Intersect({{0, 0.5, 0}, {0, 1, 0}}) << '\n';
  std::cout << c.Intersect({{0, 0.5, -5}, {0, 0, 1}}) << '\n';

  CylinderPlasmaQuartz solver;
  solver.Solve();
}
