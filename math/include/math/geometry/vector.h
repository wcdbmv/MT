#pragma once

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <limits>
#ifdef DEBUG
#include <stdexcept>
#endif

#include "base/arithmetic.h"
#include "base/noexcept_release.h"

template <std::size_t Size, Arithmetic T>
class Vector : public std::array<T, Size> {
  static_assert(Size > 0);
  using Base = std::array<T, Size>;

 public:
  constexpr Vector(std::initializer_list<T> list) noexcept;

  [[nodiscard]] constexpr T& x() noexcept;
  [[nodiscard]] constexpr T x() const noexcept;
  [[nodiscard]] constexpr T& y() noexcept
    requires(Size >= 2);
  [[nodiscard]] constexpr T y() const noexcept
    requires(Size >= 2);
  [[nodiscard]] constexpr T& z() noexcept
    requires(Size >= 3);
  [[nodiscard]] constexpr T z() const noexcept
    requires(Size >= 3);

  constexpr Vector& operator+=(const Vector& rhs) noexcept;
  constexpr Vector& operator-=(const Vector& rhs) noexcept;
  constexpr Vector& operator*=(const Vector& rhs) noexcept;
  constexpr Vector& operator/=(const Vector& rhs) NOEXCEPT_RELEASE;
  constexpr Vector& operator*=(T rhs) noexcept;
  constexpr Vector& operator/=(T rhs) NOEXCEPT_RELEASE;

  [[nodiscard]] static constexpr T Dot(const Vector& lhs,
                                       const Vector& rhs) noexcept;
  [[nodiscard]] static constexpr Vector Cross(const Vector& lhs,
                                              const Vector& rhs) noexcept
    requires(Size == 3);

  [[nodiscard]] static constexpr T SquaredDistance(const Vector& lhs,
                                                   const Vector& rhs) noexcept;
  [[nodiscard]] constexpr T SquaredLength() const noexcept;

  constexpr void Normalize() NOEXCEPT_RELEASE
    requires std::floating_point<T>;
  [[nodiscard]] constexpr Vector Normalized() const NOEXCEPT_RELEASE
    requires std::floating_point<T>;
  [[nodiscard]] constexpr bool IsNormalized() const noexcept
    requires std::floating_point<T>;
};

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T>::Vector(std::initializer_list<T> list) noexcept
    : Base{} {
  std::copy_n(list.begin(), std::min(Size, list.size()), Base::begin());
}

template <std::size_t Size, Arithmetic T>
constexpr T& Vector<Size, T>::x() noexcept {
  return (*this)[0];
}

template <std::size_t Size, Arithmetic T>
constexpr T Vector<Size, T>::x() const noexcept {
  return (*this)[0];
}

template <std::size_t Size, Arithmetic T>
constexpr T& Vector<Size, T>::y() noexcept
  requires(Size >= 2)
{
  return (*this)[1];
}

template <std::size_t Size, Arithmetic T>
constexpr T Vector<Size, T>::y() const noexcept
  requires(Size >= 2)
{
  return (*this)[1];
}

template <std::size_t Size, Arithmetic T>
constexpr T& Vector<Size, T>::z() noexcept
  requires(Size >= 3)
{
  return (*this)[2];
}

template <std::size_t Size, Arithmetic T>
constexpr T Vector<Size, T>::z() const noexcept
  requires(Size >= 3)
{
  return (*this)[2];
}

template <std::size_t Size, Arithmetic T>
constexpr auto Vector<Size, T>::operator+=(const Vector& rhs) noexcept
    -> Vector& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] += rhs[i];
  }

  return *this;
}

template <std::size_t Size, Arithmetic T>
constexpr auto Vector<Size, T>::operator-=(const Vector& rhs) noexcept
    -> Vector& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] -= rhs[i];
  }

  return *this;
}

template <std::size_t Size, Arithmetic T>
constexpr auto Vector<Size, T>::operator*=(const Vector& rhs) noexcept
    -> Vector& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] *= rhs[i];
  }

  return *this;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T>& Vector<Size, T>::operator/=(const Vector& rhs)
    NOEXCEPT_RELEASE {
  for (std::size_t i = 0; i < Size; ++i) {
#ifdef DEBUG
    if (rhs[i] == T{}) [[unlikely]] {
      throw std::overflow_error("Vector::operator/=");
    }
#endif

    (*this)[i] /= rhs[i];
  }

  return *this;
}

template <std::size_t Size, Arithmetic T>
constexpr auto Vector<Size, T>::operator*=(T rhs) noexcept -> Vector& {
  for (auto& item : *this) {
    item *= rhs;
  }

  return *this;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T>& Vector<Size, T>::operator/=(T rhs) NOEXCEPT_RELEASE {
#ifdef DEBUG
  if (rhs == T{}) [[unlikely]] {
    throw std::overflow_error("Vector::operator/");
  }
#endif

  for (auto& item : *this) {
    item *= rhs;
  }

  return *this;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator-(Vector<Size, T> lhs) noexcept {
  for (auto& item : lhs) {
    item = -item;
  }

  return lhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator+(Vector<Size, T> lhs,
                                    const Vector<Size, T>& rhs) noexcept {
  return lhs += rhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator-(Vector<Size, T> lhs,
                                    const Vector<Size, T>& rhs) noexcept {
  return lhs -= rhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator*(Vector<Size, T> lhs,
                                    const Vector<Size, T>& rhs) noexcept {
  return lhs *= rhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator/(Vector<Size, T> lhs,
                                    const Vector<Size, T>& rhs)
    NOEXCEPT_RELEASE {
  return lhs /= rhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator*(Vector<Size, T> lhs, T rhs) noexcept {
  return lhs *= rhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator*(T lhs, Vector<Size, T> rhs) noexcept {
  return rhs *= lhs;
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> operator/(Vector<Size, T> lhs,
                                    T rhs) NOEXCEPT_RELEASE {
  return lhs /= rhs;
}

template <std::size_t Size, Arithmetic T>
constexpr T Vector<Size, T>::Dot(const Vector& lhs,
                                 const Vector& rhs) noexcept {
  T product{};

  for (std::size_t i = 0; i < Size; ++i) {
    product += lhs[i] * rhs[i];
  }

  return product;
}

template <std::size_t Size, Arithmetic T>
constexpr auto Vector<Size, T>::Cross(const Vector& lhs,
                                      const Vector& rhs) noexcept -> Vector
  requires(Size == 3)
{
  // clang-format off
  return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
          lhs[2] * rhs[0] - lhs[0] * rhs[2],
          lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  // clang-format on
}

template <std::size_t Size, Arithmetic T>
constexpr T Vector<Size, T>::SquaredDistance(const Vector& lhs,
                                             const Vector& rhs) noexcept {
  return (lhs - rhs).SquaredLength();
}

template <std::size_t Size, Arithmetic T>
constexpr T Vector<Size, T>::SquaredLength() const noexcept {
  return Dot(*this, *this);
}

template <std::size_t Size, Arithmetic T>
constexpr void Vector<Size, T>::Normalize() NOEXCEPT_RELEASE
  requires std::floating_point<T>
{
  const auto squared_length = SquaredLength();
#ifdef DEBUG
  if (std::abs(squared_length) < static_cast<T>(1e-12)) [[unlikely]] {
    throw std::overflow_error("Vector::Normalize");
  }
#endif

  *this *= T{1} / std::sqrt(squared_length);
}

template <std::size_t Size, Arithmetic T>
constexpr Vector<Size, T> Vector<Size, T>::Normalized() const NOEXCEPT_RELEASE
  requires std::floating_point<T>
{
  auto normalized = *this;
  normalized.Normalize();
  return normalized;
}

template <std::size_t Size, Arithmetic T>
constexpr bool Vector<Size, T>::IsNormalized() const noexcept
  requires std::floating_point<T>
{
  return std::abs(SquaredLength() - 1) <= std::numeric_limits<T>::epsilon();
}
