#include "math/random.h"

#include <cassert>
#include <random>

Float RandFloat() noexcept {
  thread_local std::mt19937 engine{std::random_device{}()};
  thread_local std::uniform_real_distribution<Float> distribution(0, 1);

  const auto p = distribution(engine);
  return p;
}


bool ImFeelingLucky(Float probability) noexcept {
  assert(0 <= probability && probability <= 1);
  return RandFloat() <= probability;
}
