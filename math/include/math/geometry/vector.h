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

#include "base/float_cmp.h"
#include "base/noexcept_release.h"
#include "math/fast_pow.h"

template <std::size_t Size, std::floating_point T>
class Vector : public std::array<T, Size> {
  static_assert(2 <= Size && Size <= 4);
  static_assert(sizeof(std::array<T, Size>) == sizeof(T) * Size);
  using Base = std::array<T, Size>;

 public:
  constexpr Vector(std::initializer_list<T> list) noexcept;

  [[nodiscard]] constexpr T& x() noexcept;
  [[nodiscard]] constexpr T x() const noexcept;
  [[nodiscard]] constexpr T& y() noexcept requires(Size >= 2);
  [[nodiscard]] constexpr T y() const noexcept requires(Size >= 2);
  [[nodiscard]] constexpr T& z() noexcept requires(Size >= 3);
  [[nodiscard]] constexpr T z() const noexcept requires(Size >= 3);
  [[nodiscard]] constexpr T& w() noexcept requires(Size >= 4);
  [[nodiscard]] constexpr T w() const noexcept requires(Size >= 4);

  constexpr Vector& operator+=(Vector rhs) noexcept;
  constexpr Vector& operator-=(Vector rhs) noexcept;
  constexpr Vector& operator*=(Vector rhs) noexcept;
  constexpr Vector& operator/=(Vector rhs) NOEXCEPT_RELEASE;
  constexpr Vector& operator*=(T rhs) noexcept;
  constexpr Vector& operator/=(T rhs) NOEXCEPT_RELEASE;

  [[nodiscard]] static constexpr T Dot(Vector lhs, Vector rhs) noexcept;
  [[nodiscard]] static constexpr Vector Cross(Vector lhs, Vector rhs) noexcept
      requires(Size == 3);

  [[nodiscard]] static constexpr T SquaredDistance(Vector lhs,
                                                   Vector rhs) noexcept;
  [[nodiscard]] static constexpr T Distance(Vector lhs, Vector rhs) noexcept;
  [[nodiscard]] constexpr T SquaredLength() const noexcept;
  [[nodiscard]] constexpr T Length() const noexcept;

  constexpr void Normalize() NOEXCEPT_RELEASE;
  [[nodiscard]] constexpr Vector Normalized() const NOEXCEPT_RELEASE;
  [[nodiscard]] constexpr bool IsNormalized() const noexcept;
};

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T>::Vector(const std::initializer_list<T> list) noexcept
    : Base{} {
  std::copy_n(list.begin(), std::min(Size, list.size()), Base::begin());
}

template <std::size_t Size, std::floating_point T>
constexpr T& Vector<Size, T>::x() noexcept {
  return (*this)[0];
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::x() const noexcept {
  return (*this)[0];
}

template <std::size_t Size, std::floating_point T>
constexpr T& Vector<Size, T>::y() noexcept requires(Size >= 2)
{
  return (*this)[1];
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::y() const noexcept requires(Size >= 2)
{
  return (*this)[1];
}

template <std::size_t Size, std::floating_point T>
constexpr T& Vector<Size, T>::z() noexcept requires(Size >= 3)
{
  return (*this)[2];
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::z() const noexcept requires(Size >= 3)
{
  return (*this)[2];
}

template <std::size_t Size, std::floating_point T>
constexpr T& Vector<Size, T>::w() noexcept requires(Size >= 4)
{
  return (*this)[3];
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::w() const noexcept requires(Size >= 4)
{
  return (*this)[3];
}

template <std::size_t Size, std::floating_point T>
constexpr auto Vector<Size, T>::operator+=(const Vector rhs) noexcept
    -> Vector& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] += rhs[i];
  }

  return *this;
}

template <std::size_t Size, std::floating_point T>
constexpr auto Vector<Size, T>::operator-=(const Vector rhs) noexcept
    -> Vector& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] -= rhs[i];
  }

  return *this;
}

template <std::size_t Size, std::floating_point T>
constexpr auto Vector<Size, T>::operator*=(const Vector rhs) noexcept
    -> Vector& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] *= rhs[i];
  }

  return *this;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T>& Vector<Size, T>::operator/=(const Vector rhs)
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

template <std::size_t Size, std::floating_point T>
constexpr auto Vector<Size, T>::operator*=(const T rhs) noexcept -> Vector& {
  for (auto& item : *this) {
    item *= rhs;
  }

  return *this;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T>& Vector<Size, T>::operator/=(const T rhs)
    NOEXCEPT_RELEASE {
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

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator-(Vector<Size, T> lhs) noexcept {
  for (auto& item : lhs) {
    item = -item;
  }

  return lhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator+(Vector<Size, T> lhs,
                                    const Vector<Size, T> rhs) noexcept {
  return lhs += rhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator-(Vector<Size, T> lhs,
                                    const Vector<Size, T> rhs) noexcept {
  return lhs -= rhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator*(Vector<Size, T> lhs,
                                    const Vector<Size, T> rhs) noexcept {
  return lhs *= rhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator/(Vector<Size, T> lhs,
                                    const Vector<Size, T> rhs)
    NOEXCEPT_RELEASE {
  return lhs /= rhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator*(Vector<Size, T> lhs, const T rhs) noexcept {
  return lhs *= rhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator*(const T lhs, Vector<Size, T> rhs) noexcept {
  return rhs *= lhs;
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> operator/(Vector<Size, T> lhs,
                                    const T rhs) NOEXCEPT_RELEASE {
  return lhs /= rhs;
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::Dot(const Vector lhs, const Vector rhs) noexcept {
  T product{};

  for (std::size_t i = 0; i < Size; ++i) {
    product += lhs[i] * rhs[i];
  }

  return product;
}

template <std::size_t Size, std::floating_point T>
constexpr auto Vector<Size, T>::Cross(const Vector lhs,
                                      const Vector rhs) noexcept -> Vector
    requires(Size == 3)
{
  // clang-format off
  return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
          lhs[2] * rhs[0] - lhs[0] * rhs[2],
          lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  // clang-format on
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::SquaredDistance(const Vector lhs,
                                             const Vector rhs) noexcept {
  return (lhs - rhs).SquaredLength();
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::Distance(const Vector lhs,
                                      const Vector rhs) noexcept {
  return std::sqrt(SquaredDistance(lhs, rhs));
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::SquaredLength() const noexcept {
  return Dot(*this, *this);
}

template <std::size_t Size, std::floating_point T>
constexpr T Vector<Size, T>::Length() const noexcept {
  return std::sqrt(SquaredLength());
}

template <std::size_t Size, std::floating_point T>
constexpr void Vector<Size, T>::Normalize() NOEXCEPT_RELEASE {
  const auto squared_length = SquaredLength();
#ifdef DEBUG
  if (IsZero(squared_length)) [[unlikely]] {
    throw std::overflow_error("Vector::Normalize");
  }
#endif

  *this *= T{1} / std::sqrt(squared_length);
}

template <std::size_t Size, std::floating_point T>
constexpr Vector<Size, T> Vector<Size, T>::Normalized() const NOEXCEPT_RELEASE {
  auto normalized = *this;
  normalized.Normalize();
  return normalized;
}

template <std::size_t Size, std::floating_point T>
constexpr bool Vector<Size, T>::IsNormalized() const noexcept {
  return IsEqual(Length(), T{1}, std::numeric_limits<T>::epsilon());
}
