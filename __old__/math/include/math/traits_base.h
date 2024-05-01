#pragma once

#include <concepts>

#define MT_MATH_DEFINE_FUNCTION(name, func)                     \
  template <std::floating_point T>                              \
  struct name##Traits {};                                       \
                                                                \
  template <>                                                   \
  struct name##Traits<float> {                                  \
    [[nodiscard]] static float name(const float x) noexcept {   \
      return func##f(x);                                        \
    }                                                           \
  };                                                            \
                                                                \
  template <>                                                   \
  struct name##Traits<double> {                                 \
    [[nodiscard]] static double name(const double x) noexcept { \
      return func(x);                                           \
    }                                                           \
  };                                                            \
                                                                \
  template <std::floating_point T>                              \
  [[nodiscard]] T name(T x) noexcept {                          \
    return name##Traits<T>::name(x);                            \
  }
