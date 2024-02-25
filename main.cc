#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "base/erase_remove_if.h"
#include "base/float.h"
#include "base/float_cmp.h"
#include "math/geometry/cylinder_z_infinite.h"
#include "math/geometry/disk.h"
#include "math/geometry/fibonacci_sphere.h"
#include "math/geometry/ray.h"
#include "math/geometry/vector.h"
#include "math/geometry/vector3f.h"
#include "math/geometry/vector_io.h"
#include "math/random.h"
#include "math/utils.h"
#include "physics/params.h"
#include "physics/plancks_law.h"

namespace {

constexpr auto kStep = static_cast<Float>(0.05);
// NOLINTNEXTLINE(cert-err58-cpp)
const auto kN = static_cast<size_t>(std::round(params::R_1 / kStep));
// NOLINTNEXTLINE(cert-err58-cpp)
const auto kPlasmaIdx = static_cast<size_t>(std::round(params::R / kStep) - 1);

struct CylinderPlasmaQuartz {
  std::vector<CylinderZInfinite> cylinders;
  std::vector<Float> temperatures;
  std::vector<Float> intensities;
  std::array<Disk, 2> borders;

  CylinderPlasmaQuartz() : borders{InitBorders()} {
    InitCylinders();

    assert(kN == 9);
  }

 private:
  void InitCylinders() {
    cylinders.reserve(kN);
    temperatures.reserve(kN);
    intensities.reserve(kN);

    for (size_t i = 1; i <= kPlasmaIdx; ++i) {
      const auto radius = kStep * static_cast<Float>(i);
      cylinders.emplace_back(kOrigin, radius);
      const auto center =
          kStep * (static_cast<Float>(i) - static_cast<Float>(0.5));
      temperatures.push_back(params::T(center / params::R));
      intensities.push_back(func::I(params::nu, temperatures.back()));
    }

    cylinders.emplace_back(kOrigin, params::R);
    assert(cylinders.size() == kPlasmaIdx + 1);
    temperatures.push_back(params::T((params::R - kStep / 2) / params::R));
    assert(temperatures.size() == kPlasmaIdx + 1);
    intensities.push_back(func::I(params::nu, temperatures.back()));
    assert(intensities.size() == kPlasmaIdx + 1);

    for (size_t i = kPlasmaIdx + 2; i < kN; ++i) {
      const auto radius = kStep * static_cast<Float>(i);
      cylinders.emplace_back(kOrigin, radius);
      const auto center =
          kStep * (static_cast<Float>(i) - static_cast<Float>(0.5));
      temperatures.push_back(params::T(center / params::R));
      intensities.push_back(func::I(params::nu, temperatures.back()));
    }

    cylinders.emplace_back(kOrigin, params::R_1);
    temperatures.emplace_back(params::T((params::R_1 - kStep / 2) / params::R));
    intensities.push_back(func::I(params::nu, temperatures.back()));

    std::cout << "Cylinders:\n";
    for (auto& cylinder : cylinders) {
      std::cout << cylinder.center() << ' ' << std::sqrt(cylinder.radius2())
                << '\n';
    }

    std::cout << "T:\n";
    for (const auto t : temperatures) {
      std::cout << t << '\n';
    }

    std::cout << "I:\n";
    for (const auto i : intensities) {
      std::cout << i << '\n';
    }
  }

  [[nodiscard]] static std::array<Disk, 2> InitBorders() {
    return {
        Disk{kOrigin, -kUnitZ, params::R_1},
        Disk{kOrigin + Vector3F{0, 0, params::H}, kUnitZ, params::R_1},
    };
  }
};

class Worker {
 public:
  explicit constexpr Worker(const CylinderPlasmaQuartz& c) noexcept : c_{c} {}

  std::vector<Float> SolveDir(const Vector3F dir, Float intensity) {
    std::vector<Float> absorbed(c_.cylinders.size());
    const auto initial_intensity = intensity;

    current_cylinder_idx_ = kPlasmaIdx;

    constexpr auto kInitialPos = Vector3F{params::R, 0, params::H / 2};
    ray_ = {kInitialPos, dir};

    auto use_prev = false;

    for (size_t i = 0; intensity > 0.01 * initial_intensity; ++i) {
      std::cout << '[' << i << ", " << intensity << "]\n";

      IntersectPrevCylinder();
      IntersectNextCylinder();
      IntersectCurrCylinder();

      const auto t_min_idx = FindMinimalNonNegativeIndex(ts_);
      if (t_min_idx <= kIdxLastCylinder) {
        const auto prev_pos = ray_.pos;
        ray_.pos = ray_.Point(ts_[t_min_idx]);

        const auto prev_cylinder_idx = current_cylinder_idx_;

        if (t_min_idx == kIdxPrevCylinder) {
          --current_cylinder_idx_;
        } else if (t_min_idx == kIdxNextCylinder) {
          ++current_cylinder_idx_;
        } else {
          use_prev = !use_prev;
        }

        const auto idx = use_prev ? prev_cylinder_idx : current_cylinder_idx_;
        const auto T = c_.temperatures[idx];
        const auto dr = Vector3F::Distance(prev_pos, ray_.pos);
        const auto exp = std::exp(-params::k_plasma(T) * dr);
        const auto prev_intensity = intensity;
        intensity *= exp;
        absorbed[idx] += prev_intensity - intensity;

        std::cout << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx
                  << "][ci=" << current_cylinder_idx_ << "]\n";

        if (current_cylinder_idx_ + 1 == c_.cylinders.size()) {
          if (!ImFeelingLucky(params::rho)) {
            std::cout << "ABSORPTION at the quartz boundary\n";
            break;
          }
          ray_.dir = c_.cylinders.back().Reflect(ray_.pos, ray_.dir);
          std::cout << "REFLECT QUARTZ, new dir " << ray_.dir << '\n';
        }

      } else {
        throw std::out_of_range("ALERT FAILURE ERROR");
      }
    }

    return absorbed;
  }

  Float CalculateIntensity(const Vector3F dir) {
    const auto ndir = -dir;

    constexpr auto kInitialPos = Vector3F{params::R, 0, params::H / 2};
    ray_ = {kInitialPos + ndir * (1 / static_cast<Float>(1024)), ndir};
    const auto t = c_.cylinders[kPlasmaIdx].Intersect(ray_);
    assert(t > 0);

    ray_.pos = ray_.Point(t);
    ray_.dir = dir;

    current_cylinder_idx_ = kPlasmaIdx;
    Float intensity{};
    auto use_prev = true;

    for (size_t i = 0;; ++i) {
      std::cout << "[Intensity, " << i << ", " << intensity << "]\n";

      IntersectPrevCylinder();
      IntersectNextCylinder();
      IntersectCurrCylinder();

      const auto t_min_idx = FindMinimalNonNegativeIndex(ts_);
      if (t_min_idx <= kIdxLastCylinder) {
        const auto prev_pos = ray_.pos;
        ray_.pos = ray_.Point(ts_[t_min_idx]);
        if (ts_[t_min_idx] < 3E-16) {
          [[maybe_unused]] const int klsdf = 0;
        }

        const auto prev_cylinder_idx = current_cylinder_idx_;

        if (t_min_idx == kIdxPrevCylinder) {
          --current_cylinder_idx_;
        } else if (t_min_idx == kIdxNextCylinder) {
          ++current_cylinder_idx_;
        } else {
          use_prev = false;
        }

        const auto idx = use_prev ? prev_cylinder_idx : current_cylinder_idx_;
        const auto T = c_.temperatures[idx];
        const auto dr = Vector3F::Distance(prev_pos, ray_.pos);
        const auto exp = std::exp(-params::k_plasma(T) * dr);
        intensity *= exp;
        intensity += c_.intensities[idx] * (1 - exp);

        std::cout << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx
                  << "][ci=" << current_cylinder_idx_ << "]\n";

        if (current_cylinder_idx_ == kPlasmaIdx) {
          break;
        }
        if (current_cylinder_idx_ > kPlasmaIdx) {
          throw std::out_of_range("ALERT RAY OUT OF PLASMA");
        }
      } else {
        throw std::out_of_range("ALERT FAILURE ERROR");
      }
    }

    return intensity;
  }

 private:
  void PrintT(const char* prompt, const Float t) const {
    std::cout << prompt << t;
    if (t > kEps) {
      std::cout << ' ' << ray_.Point(t);
    }
    std::cout << '\n';
  }

  void IntersectPrevCylinder() {
    if (current_cylinder_idx_ > 0) {
      const auto t = c_.cylinders[current_cylinder_idx_ - 1].Intersect(ray_);
      ts_[kIdxPrevCylinder] = t;

      PrintT("PrevCylinder  ", t);
    } else {
      ts_[kIdxPrevCylinder] = -1;
    }
  }

  void IntersectNextCylinder() {
    if (current_cylinder_idx_ + 1 < c_.cylinders.size()) {
      const auto t = c_.cylinders[current_cylinder_idx_ + 1].Intersect(ray_);
      ts_[kIdxNextCylinder] = t;

      PrintT("NextCylinder  ", t);
    } else {
      ts_[kIdxNextCylinder] = -1;
    }
  }

  void IntersectCurrCylinder() {
    const auto t = c_.cylinders[current_cylinder_idx_].Intersect(ray_);
    ts_[kIdxCurrCylinder] = IsZero(t) ? -1 : t;

    PrintT("CurrCylinder* ", t);
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  const CylinderPlasmaQuartz& c_;

  Ray ray_{};
  size_t current_cylinder_idx_{};

  static constexpr size_t kIdxPrevCylinder = 0;
  static constexpr size_t kIdxNextCylinder = 1;
  static constexpr size_t kIdxCurrCylinder = 2;
  static constexpr size_t kIdxLastCylinder = kIdxCurrCylinder;
  std::array<Float, 3> ts_{};
};

class Solver {
 public:
  Solver() { InitDirs(); }

  void Solve() const {
    Worker worker{cpq_};

    std::vector<Float> total_absorbed(cpq_.cylinders.size());

    for (const auto dir : dirs_) {
      const auto I = worker.CalculateIntensity(dir);
      const auto absorbed = worker.SolveDir(dir, I);

      std::cout << "ABSORBED:\n";
      for (size_t i = 0; i < absorbed.size(); ++i) {
        total_absorbed[i] += absorbed[i];
        std::cout << absorbed[i] << '\n';
      }
    }

    std::cout << "TOTAL ABSORBED:\n";
    for (const auto i : total_absorbed) {
      std::cout << i << '\n';
    }
  }

 private:
  void InitDirs() {
    dirs_ = FibonacciSphere(200);
    EraseRemoveIf(dirs_, [](const Vector3F dir) { return dir.x() <= 0; });
    for (const auto dir : dirs_) {
      std::cout << dir << '\n';
    }
  }

  CylinderPlasmaQuartz cpq_;
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

  const Solver solver;
  solver.Solve();
}
