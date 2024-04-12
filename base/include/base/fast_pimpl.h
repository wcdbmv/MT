#pragma once

#include <array>
#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

template <typename T, std::size_t Size, std::size_t Alignment>
class FastPimpl final {
 public:
  FastPimpl(FastPimpl&& v) noexcept(noexcept(T(std::declval<T>())))
      : FastPimpl(std::move(*v)) {}

  FastPimpl(const FastPimpl& v) noexcept(noexcept(T(std::declval<const T&>())))
      : FastPimpl(*v) {}

  FastPimpl& operator=(const FastPimpl& rhs) noexcept(
      noexcept(std::declval<T&>() = std::declval<const T&>())) {
    if (this != &rhs) {
      *AsHeld() = *rhs;
    }
    return *this;
  }

  FastPimpl& operator=(FastPimpl&& rhs) noexcept(
      noexcept(std::declval<T&>() = std::declval<T>())) {
    *AsHeld() = std::move(*rhs);
    return *this;
  }

  template <typename... Args>
  explicit FastPimpl(Args&&... args) noexcept(
      noexcept(T(std::declval<Args>()...))) {
    new (AsHeld()) T(std::forward<Args>(args)...);
  }

  T* operator->() noexcept { return AsHeld(); }

  const T* operator->() const noexcept { return AsHeld(); }

  T& operator*() noexcept { return *AsHeld(); }

  const T& operator*() const noexcept { return *AsHeld(); }

  ~FastPimpl() noexcept {
    Validate<sizeof(T), alignof(T)>();
    AsHeld()->~T();
  }

 private:
  // Use a template to make actual sizes visible in the compiler error message.
  template <std::size_t ActualSize, std::size_t ActualAlignment>
  static void Validate() noexcept {
    static_assert(Size == sizeof storage_,
                  "invalid Size: Size == sizeof(storage_) failed");

    static_assert(Size == ActualSize, "invalid Size: Size == sizeof(T) failed");

    static_assert(Alignment == ActualAlignment,
                  "invalid Alignment: Alignment == alignof(T) failed");
  }

  alignas(Alignment) std::array<std::byte, Size> storage_;

  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  T* AsHeld() noexcept { return reinterpret_cast<T*>(storage_.data()); }

  const T* AsHeld() const noexcept {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<const T*>(storage_.data());
  }
};
