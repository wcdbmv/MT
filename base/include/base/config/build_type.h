#pragma once

#ifdef NDEBUG
#define MT_RELEASE
#else
#define MT_DEBUG
#endif

namespace config {

[[nodiscard]] constexpr bool IsRelease() noexcept {
#ifdef MT_RELEASE
  return true;
#else
  return false;
#endif
}

[[nodiscard]] constexpr bool IsDebug() noexcept {
  return !IsRelease();
}

}  // namespace config
