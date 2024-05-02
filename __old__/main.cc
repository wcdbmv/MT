#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <format>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base/erase_remove_if.h"
#include "base/float.h"
#include "base/float_cmp.h"
#include "geogebra/api.h"
#include "math/consts/pi.h"
#include "math/exp.h"
#include "math/fast_pow.h"
#include "math/linalg/ray.h"
#include "math/linalg/vector.h"
#include "math/linalg/vector3f.h"
#include "math/random.h"
#include "math/sqrt.h"
#include "math/utils.h"
#include "modeling/fibonacci_sphere.h"
#include "physics/params.h"
#include "physics/plancks_law.h"
#include "ray_tracing/cylinder_z_infinite.h"
#include "ray_tracing/disk.h"

#define ENABLE_DEBUG_OUTPUT
// #define ENABLE_GEOGEBRA_OUTPUT
// #define ENABLE_GEOGEBRA_OUTPUT_SPHERE

#ifdef ENABLE_DEBUG_OUTPUT
#include "math/linalg/vector_io.h"
#endif

namespace {

struct DummyOut {};
[[maybe_unused]] DummyOut gDummyOut;  // NOLINT

template <typename T>
DummyOut& operator<<(DummyOut& dummy_out, T&&) noexcept {  // NOLINT
  return dummy_out;
}

#ifdef ENABLE_DEBUG_OUTPUT
#define DEBUG_OUT std::cout
#else
#define DEBUG_OUT gDummyOut
#endif

#ifdef ENABLE_GEOGEBRA_OUTPUT
#define GEOGEBRA_OUT std::cout
#else
#define GEOGEBRA_OUT gDummyOut
#endif

constexpr auto kStep = static_cast<Float>(0.05);
// NOLINTNEXTLINE(cert-err58-cpp)
const auto kN = static_cast<size_t>(std::round(params::R_1 / kStep));
// NOLINTNEXTLINE(cert-err58-cpp)
const auto kPlasmaIdx = static_cast<size_t>(std::round(params::R / kStep) - 1);

// TODO(a.kerimov): Выяснить, что происходит при 400000+ и CONSTANT_TEMPERATURE
constexpr auto kSpherePoints = 100;

// TODO(a.kerimov): Написать тесты.

// TODO(a.kerimov): остаток интенсивности сохранить

struct StartParams {
  Ray ray;
  Float intensity{};
  Float intensity_end{};
  bool use_prev{false};
};

struct CylinderPlasmaQuartz {
  std::vector<CylinderZInfinite> cylinders;
  std::vector<Float> temperatures;
  std::vector<Float> intensities;

  CylinderPlasmaQuartz() {
    InitCylinders();

    assert(kPlasmaIdx == 6);
    assert(kN == 9);
    // assert(kPlasmaIdx == 34);
    // assert(kN == 45);
  }

 private:
  void InitCylinders() {
    cylinders.reserve(kN);
    temperatures.reserve(kN);
    intensities.reserve(kN);

    for (size_t i = 1; i <= kPlasmaIdx; ++i) {
      const auto radius = kStep * static_cast<Float>(i);
      cylinders.emplace_back(kOrigin, radius);
      GEOGEBRA_OUT << geogebra::CylinderInfiniteZ(std::format("C{}", i - 1),
                                                  radius);
      const auto center =
          kStep * (static_cast<Float>(i) - static_cast<Float>(0.5));
      temperatures.push_back(params::T(center / params::R));
      intensities.push_back(func::I(params::nu, temperatures.back()));
    }

    cylinders.emplace_back(kOrigin, params::R);
    GEOGEBRA_OUT << geogebra::CylinderInfiniteZ(std::format("C{}", kPlasmaIdx),
                                                params::R);
    assert(cylinders.size() == kPlasmaIdx + 1);
    temperatures.push_back(params::T((params::R - kStep / 2) / params::R));
    assert(temperatures.size() == kPlasmaIdx + 1);
    intensities.push_back(func::I(params::nu, temperatures.back()));
    assert(intensities.size() == kPlasmaIdx + 1);
#ifdef CONSTANT_TEMPERATURE
    std::cout << "INTENSITY: " << intensities.back() << "\n";
#endif

    for (size_t i = kPlasmaIdx + 2; i < kN; ++i) {
      const auto radius = kStep * static_cast<Float>(i);
      cylinders.emplace_back(kOrigin, radius);
      GEOGEBRA_OUT << geogebra::CylinderInfiniteZ(std::format("C{}", i - 1),
                                                  radius);
      const auto center =
          kStep * (static_cast<Float>(i) - static_cast<Float>(0.5));
      temperatures.push_back(params::T(center / params::R));
      intensities.push_back(func::I(params::nu, temperatures.back()));
    }

    cylinders.emplace_back(kOrigin, params::R_1);
    GEOGEBRA_OUT << geogebra::CylinderInfiniteZ(
        std::format("C{}", cylinders.size() - 1), params::R_1);
    temperatures.emplace_back(params::T((params::R_1 - kStep / 2) / params::R));
    intensities.push_back(func::I(params::nu, temperatures.back()));
#ifdef CONSTANT_TEMPERATURE
    std::cout << "INTENSITY: " << intensities.back() << "\n";
#endif

    DEBUG_OUT << "Cylinders:\n";
    for (auto& cylinder : cylinders) {
      DEBUG_OUT << cylinder.center() << ' ' << Sqrt(cylinder.radius2()) << '\n';
    }

    DEBUG_OUT << "T:\n";
    for (const auto t : temperatures) {
      DEBUG_OUT << t << '\n';
    }

    DEBUG_OUT << "I:\n";
    for (const auto i : intensities) {
      DEBUG_OUT << i << '\n';
    }
  }
};

Float kIntensityAll = 0;     // NOLINT
Float kIntensityQuartz = 0;  // NOLINT

class Worker {
 public:
  explicit constexpr Worker(const CylinderPlasmaQuartz& c,
                            std::vector<StartParams>& wait) noexcept
      : c_{c}, wait_{wait} {}

  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
  std::vector<Float> SolveDir(const StartParams& params) {
    std::vector<Float> absorbed(c_.cylinders.size());
    auto intensity = params.intensity;

    current_cylinder_idx_ = kPlasmaIdx;

    ray_ = params.ray;

    auto dir_idx = 0;
    GEOGEBRA_OUT << geogebra::Point3D("P0", ray_.pos);
    GEOGEBRA_OUT << geogebra::Point3D("D0", ray_.dir);
    GEOGEBRA_OUT << geogebra::Ray3D("R0", "P0", "D0");

    auto use_prev = params.use_prev;

    for (size_t i = 0; intensity > params.intensity_end; ++i) {
      DEBUG_OUT << '[' << i << ", " << intensity << "]\n";

      IntersectPrevCylinder();
      IntersectNextCylinder();
      IntersectCurrCylinder();

      const auto t_min_idx = FindIndexOfMinimalNonNegative(ts_);
      if (t_min_idx <= kIdxLastCylinder) {
        const auto prev_pos = ray_.pos;
        ray_.pos = ray_.Point(ts_[t_min_idx]);

        const auto Pi = std::format("P{}", i + 1);
        GEOGEBRA_OUT << geogebra::Point3D(Pi, ray_.pos);

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
        const auto k =
            idx > kPlasmaIdx ? params::k_quartz(T) : params::k_plasma(T);
        const auto exp = Exp(-k * dr);
        const auto prev_intensity = intensity;
        intensity *= exp;
        absorbed[idx] += prev_intensity - intensity;

        DEBUG_OUT << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx
                  << "][ci=" << current_cylinder_idx_ << "][dir=" << ray_.dir
                  << "]\n";

        if (current_cylinder_idx_ == kPlasmaIdx) {
          auto n_1 = params::n_plasma;
          auto n_2 = params::n_quartz;
          const auto outward = prev_cylinder_idx <= current_cylinder_idx_;
          if (!outward) {
            std::swap(n_1, n_2);
          }

          const auto res =
              c_.cylinders[kPlasmaIdx].Refract(ray_, n_1, n_2, 0, outward);

          // TODO(a.kerimov): Mutex.
          if (res.T > 0) {
            if (const auto IT = intensity * res.T; IT > params.intensity_end) {
              wait_.push_back({{ray_.pos, res.refracted},
                               intensity * res.T,
                               params.intensity_end,
                               use_prev});
            } else {
              absorbed[kPlasmaIdx + 1] += IT;  // +1 or no ?????
            }
          }

          assert(res.R > 0);
          ray_.dir = res.reflected;
          intensity *= res.R;
          use_prev = !use_prev;
          DEBUG_OUT << "REFLECT PLASMA, new dir " << ray_.dir << '\n';
        }

        if (current_cylinder_idx_ + 1 == c_.cylinders.size()) {
          // if (!ImFeelingLucky(params::rho)) {
          //   DEBUG_OUT << "ABSORPTION at the quartz boundary\n";
          //   break;
          // }
          // TODO(a.kerimov): rename.
          const auto prevv_intensity = intensity;
          intensity *= params::rho;
          kIntensityQuartz += prevv_intensity - intensity;

          ray_.dir = c_.cylinders.back().ReflectInside(ray_);
          use_prev = true;

          ++dir_idx;
          const auto Di = std::format("D{}", dir_idx);
          GEOGEBRA_OUT << geogebra::Point3D(Pi, ray_.pos);
          GEOGEBRA_OUT << geogebra::Point3D(Di, ray_.dir);
          GEOGEBRA_OUT << geogebra::Ray3D(std::format("R{}", dir_idx), Pi, Di);

          DEBUG_OUT << "REFLECT QUARTZ, new dir " << ray_.dir << '\n';
        }

      } else {
        throw std::out_of_range("ALERT FAILURE ERROR");
      }
    }

    // TODO(a.kerimov): Refactor.
    {
      DEBUG_OUT << "[LAST, " << intensity << "]\n";

      IntersectPrevCylinder();
      IntersectNextCylinder();
      IntersectCurrCylinder();

      const auto t_min_idx = FindIndexOfMinimalNonNegative(ts_);
      if (t_min_idx <= kIdxLastCylinder) {
        ray_.pos = ray_.Point(ts_[t_min_idx]);

        // const auto Pi = std::format("P{}", i + 1);
        // GEOGEBRA_OUT << geogebra::Point3D(Pi, ray_.pos);

        const auto prev_cylinder_idx = current_cylinder_idx_;

        if (t_min_idx == kIdxPrevCylinder) {
          --current_cylinder_idx_;
        } else if (t_min_idx == kIdxNextCylinder) {
          ++current_cylinder_idx_;
        } else {
          use_prev = !use_prev;
        }

        const auto idx = use_prev ? prev_cylinder_idx : current_cylinder_idx_;
        absorbed[idx] += intensity;
      }
    }

    return absorbed;
  }

  Float CalculateIntensity(const Vector3F dir) {
    const auto ndir = -dir;

    constexpr auto kInitialPos = Vector3F{params::R, 0, params::H / 2};
    // GEOGEBRA_OUT << geogebra::Point3D("P0", kInitialPos);
    // GEOGEBRA_OUT << geogebra::Point3D("D0", ndir);
    // GEOGEBRA_OUT << geogebra::Ray3D("R0", "P0", "D0");

    ray_ = {kInitialPos, ndir};
    const auto t = c_.cylinders[kPlasmaIdx].IntersectCurr(ray_);
    assert(t > 0);

    ray_.pos = ray_.Point(t);
    ray_.dir = dir;

    // GEOGEBRA_OUT << geogebra::Point3D("P1", ray_.pos);
    // GEOGEBRA_OUT << geogebra::Point3D("D1", ray_.dir);
    // GEOGEBRA_OUT << geogebra::Ray3D("R1", "P1", "D1");

    current_cylinder_idx_ = kPlasmaIdx;
    Float intensity{};
    auto use_prev = true;

    for (size_t i = 0;; ++i) {
      DEBUG_OUT << "[Intensity, " << i << ", " << intensity << "]\n";

      IntersectPrevCylinder();
      IntersectNextCylinder();
      IntersectCurrCylinder();

      const auto t_min_idx = FindIndexOfMinimalNonNegative(ts_);
      if (t_min_idx <= kIdxLastCylinder) {
        const auto prev_pos = ray_.pos;
        ray_.pos = ray_.Point(ts_[t_min_idx]);
        // GEOGEBRA_OUT << geogebra::Point3D(std::format("P{}", i + 2),
        // ray_.pos);
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
        const auto exp = Exp(-params::k_plasma(T) * dr);
        // #ifndef CONSTANT_TEMPERATURE
        const auto expp = exp;
        // #else
        //        (void)exp;
        //        const auto expp = 0.0;
        // #endif
        intensity *= expp;
        intensity += c_.intensities[idx] * (1 - expp);

        DEBUG_OUT << "NEW POS: " << ray_.pos << " [ti=" << t_min_idx
                  << "][ci=" << current_cylinder_idx_ << "][dir=" << ray_.dir
                  << "]\n";

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

    // const auto cos_theta = dir.z();
    // const auto sin_phi = Sqrt(Sqr(dir.x()) + Sqr(dir.y()));

    std::cout << "INTENSITY BEFORE COSINE: " << intensity << "\n";

    intensity *= 2 * 2 * consts::pi / kSpherePoints * dir.x();

    kIntensityAll += intensity;

    return intensity;
  }

 private:
  void PrintT(const char* prompt, const Float t) const {
    DEBUG_OUT << prompt << t;
    if (t > kEps) {
      DEBUG_OUT << ' ' << ray_.Point(t);
    }
    DEBUG_OUT << '\n';
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
    const auto t = c_.cylinders[current_cylinder_idx_].IntersectCurr(ray_);
    ts_[kIdxCurrCylinder] = IsZero(t, kEps) ? -1 : t;

    PrintT("CurrCylinder* ", t);
  }

  // NOLINTBEGIN(cppcoreguidelines-avoid-const-or-ref-data-members)
  const CylinderPlasmaQuartz& c_;
  std::vector<StartParams>& wait_;
  // NOLINTEND(cppcoreguidelines-avoid-const-or-ref-data-members)

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

  void Solve() {
    std::vector<Float> total_absorbed(cpq_.cylinders.size());

    for (const auto dir : dirs_) {
      Worker worker{cpq_, wait_};
      const auto I = worker.CalculateIntensity(dir);
      constexpr auto kInitialPos = Vector3F{params::R, 0, params::H / 2};
      const auto absorbed = worker.SolveDir({{kInitialPos, dir}, I, 0.01 * I});

      DEBUG_OUT << "ABSORBED:\n";
      for (size_t i = 0; i < absorbed.size(); ++i) {
        total_absorbed[i] += absorbed[i];
        DEBUG_OUT << absorbed[i] << '\n';
      }
    }

    DEBUG_OUT << "[[wait]]\n";
    // NOLINTNEXTLINE(modernize-loop-convert)
    for (size_t i = 0; i < wait_.size(); ++i) {
      Worker worker{cpq_, wait_};
      const auto absorbed = worker.SolveDir(wait_[i]);

      DEBUG_OUT << "ABSORBED:\n";
      for (size_t j = 0; j < absorbed.size(); ++j) {
        total_absorbed[j] += absorbed[j];
        DEBUG_OUT << absorbed[j] << '\n';
      }
    }

    Float total = 0;
    std::cout << "TOTAL ABSORBED:\n";
    for (const auto i : total_absorbed) {
      std::cout << i << '\n';
      total += i;
    }
    std::cout << "ABSORBED QUARTZ: " << kIntensityQuartz << '\n';
    std::cout << "SUM: " << total + kIntensityQuartz << '\n';
  }

 private:
  void InitDirs() {
    dirs_ = FibonacciSphere(kSpherePoints);
    EraseRemoveIf(dirs_, [](const Vector3F dir) { return dir.x() <= 0; });
    for (const auto dir : dirs_) {
      DEBUG_OUT << dir << '\n';
#ifdef ENABLE_GEOGEBRA_OUTPUT_SPHERE
      static int i = 0;
      std::cout << geogebra::Point3D(std::format("P{}", i), dir);
      ++i;
#endif
    }
  }

  CylinderPlasmaQuartz cpq_;
  std::vector<StartParams> wait_;
  std::vector<Vector3F> dirs_;
};

}  // namespace

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
  DEBUG_OUT << params::T(0) << '\n';
  DEBUG_OUT << params::T(1) << '\n';
  DEBUG_OUT << params::T(params::R_1 / params::R) << '\n';

  DEBUG_OUT << Vector<2, Float>{-1}.x() << '\n';
  DEBUG_OUT << Vector<2, Float>{1}.y() << '\n';

  const CylinderZInfinite c{Vector3F{}, 1};
  DEBUG_OUT << c.Intersect({{0, 0.5, 0}, {1, 0, 0}}) << '\n';
  DEBUG_OUT << c.Intersect({{0, 0.5, 0}, {0, 1, 0}}) << '\n';
  DEBUG_OUT << c.Intersect({{0, 0.5, -5}, {0, 0, 1}}) << '\n';

  Solver solver;
  solver.Solve();

  std::cout << "INTENSITY: " << kIntensityAll << '\n';
}
#endif
