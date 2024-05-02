#pragma once

template <typename T>
constexpr void IgnoreUnused(const T& /* unused */) {}

// Not working for GCC & MSVC
// clang-format off
#define MT_IGNORE_UNUSED_BEGIN     \
  _Pragma("clang diagnostic push") \
  _Pragma("clang diagnostic ignored \"-Wunused-function\"")

#define MT_IGNORE_UNUSED_END \
  _Pragma("clang diagnostic pop")
// clang-format on
