#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>

#include "base/config/float.h"
#include "base/config/noexcept_release.h"
#include "math/float/compare.h"
#include "math/float/sqrt.h"

template <std::size_t Size>
class Vec : public std::array<Float, Size> {
  static_assert(2 <= Size && Size <= 3);
  static_assert(sizeof(std::array<Float, Size>) == sizeof(Float) * Size);
  using Base = std::array<Float, Size>;

 public:
  constexpr Vec() noexcept;
  constexpr Vec(std::initializer_list<Float> list) noexcept;

  constexpr Vec(Vec a, Vec b) noexcept;

  [[nodiscard]] constexpr Float& x() noexcept;
  [[nodiscard]] constexpr Float x() const noexcept;
  [[nodiscard]] constexpr Float& y() noexcept requires(Size >= 2);
  [[nodiscard]] constexpr Float y() const noexcept requires(Size >= 2);
  [[nodiscard]] constexpr Float& z() noexcept requires(Size >= 3);
  [[nodiscard]] constexpr Float z() const noexcept requires(Size >= 3);

  constexpr void Negate() noexcept;

  constexpr Vec& operator+=(Vec rhs) noexcept;
  constexpr Vec& operator-=(Vec rhs) noexcept;
  constexpr Vec& operator*=(Float rhs) noexcept;
  constexpr Vec& operator/=(Float rhs) MT_NOEXCEPT_RELEASE;

  [[nodiscard]] static constexpr Float Dot(Vec lhs, Vec rhs) noexcept;
  [[nodiscard]] static constexpr Vec Cross(Vec lhs, Vec rhs) noexcept
      requires(Size == 3);
  [[nodiscard]] static constexpr Float Skew(Vec lhs, Vec rhs) noexcept
      requires(Size == 2);

  [[nodiscard]] static constexpr Float SquaredDistance(Vec lhs,
                                                       Vec rhs) noexcept;
  [[nodiscard]] static constexpr Float Distance(Vec lhs, Vec rhs) noexcept;
  [[nodiscard]] constexpr Float SquaredLength() const noexcept;
  [[nodiscard]] constexpr Float Length() const noexcept;

  constexpr void Normalize() MT_NOEXCEPT_RELEASE;
  [[nodiscard]] constexpr Vec Normalized() const MT_NOEXCEPT_RELEASE;
  [[nodiscard]] constexpr bool IsNormalized() const noexcept;
};

template <std::size_t Size>
constexpr Vec<Size>::Vec() noexcept : Base{} {}

template <std::size_t Size>
constexpr Vec<Size>::Vec(std::initializer_list<Float> list) noexcept : Base{} {
  std::copy_n(list.begin(), std::min(Size, list.size()), Base::begin());
}

template <std::size_t Size>
constexpr Vec<Size>::Vec(Vec a, Vec b) noexcept : Vec{b - a} {}

template <std::size_t Size>
constexpr Float& Vec<Size>::x() noexcept {
  return (*this)[0];
}

template <std::size_t Size>
constexpr Float Vec<Size>::x() const noexcept {
  return (*this)[0];
}

template <std::size_t Size>
constexpr Float& Vec<Size>::y() noexcept requires(Size >= 2)
{
  return (*this)[1];
}

template <std::size_t Size>
constexpr Float Vec<Size>::y() const noexcept requires(Size >= 2)
{
  return (*this)[1];
}

template <std::size_t Size>
constexpr Float& Vec<Size>::z() noexcept requires(Size >= 3)
{
  return (*this)[2];
}

template <std::size_t Size>
constexpr Float Vec<Size>::z() const noexcept requires(Size >= 3)
{
  return (*this)[2];
}

template <std::size_t Size>
constexpr void Vec<Size>::Negate() noexcept {
  for (auto& coord : *this) {
    coord = -coord;
  }
}

template <std::size_t Size>
constexpr auto Vec<Size>::operator+=(Vec rhs) noexcept -> Vec& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] += rhs[i];
  }

  return *this;
}

template <std::size_t Size>
constexpr auto Vec<Size>::operator-=(Vec rhs) noexcept -> Vec& {
  for (std::size_t i = 0; i < Size; ++i) {
    (*this)[i] -= rhs[i];
  }

  return *this;
}

template <std::size_t Size>
constexpr auto Vec<Size>::operator*=(Float rhs) noexcept -> Vec& {
  for (auto& item : *this) {
    item *= rhs;
  }

  return *this;
}

template <std::size_t Size>
constexpr Vec<Size>& Vec<Size>::operator/=(Float rhs) MT_NOEXCEPT_RELEASE {
  if constexpr (config::IsDebug()) {
    if (IsZero(rhs)) [[unlikely]] {
      throw std::overflow_error("Vec::operator/");
    }
  }

  for (auto& item : *this) {
    item *= rhs;
  }

  return *this;
}

template <std::size_t Size>
constexpr Vec<Size> operator-(Vec<Size> v) noexcept {
  v.Negate();
  return v;
}

template <std::size_t Size>
constexpr Vec<Size> operator+(Vec<Size> lhs, Vec<Size> rhs) noexcept {
  lhs += rhs;
  return lhs;
}

template <std::size_t Size>
constexpr Vec<Size> operator-(Vec<Size> lhs, Vec<Size> rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

template <std::size_t Size>
constexpr Vec<Size> operator*(Vec<Size> lhs, Vec<Size> rhs) noexcept {
  return Dot(lhs, rhs);
}

template <std::size_t Size>
constexpr Vec<Size> operator*(Vec<Size> lhs, Float rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

template <std::size_t Size>
constexpr Vec<Size> operator*(Float lhs, Vec<Size> rhs) noexcept {
  rhs *= lhs;
  return rhs;
}

template <std::size_t Size>
constexpr Vec<Size> operator/(Vec<Size> lhs, Float rhs) MT_NOEXCEPT_RELEASE {
  lhs /= rhs;
  return lhs;
}

template <std::size_t Size>
constexpr Float Vec<Size>::Dot(Vec lhs, Vec rhs) noexcept {
  auto product = kZero;

  for (std::size_t i = 0; i < Size; ++i) {
    product += lhs[i] * rhs[i];
  }

  return product;
}

template <std::size_t Size>
constexpr auto Vec<Size>::Cross(Vec lhs, Vec rhs) noexcept -> Vec
    requires(Size == 3)
{
  // clang-format off
  return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
          lhs[2] * rhs[0] - lhs[0] * rhs[2],
          lhs[0] * rhs[1] - lhs[1] * rhs[0]};
  // clang-format on
}

template <std::size_t Size>
constexpr Float Vec<Size>::Skew(Vec lhs, Vec rhs) noexcept requires(Size == 2)
{
  return lhs[0] * rhs[1] - lhs[1] * rhs[0];
}

template <std::size_t Size>
constexpr Float Vec<Size>::SquaredDistance(Vec lhs, Vec rhs) noexcept {
  return (lhs - rhs).SquaredLength();
}

template <std::size_t Size>
constexpr Float Vec<Size>::Distance(Vec lhs, Vec rhs) noexcept {
  return Sqrt(SquaredDistance(lhs, rhs));
}

template <std::size_t Size>
constexpr Float Vec<Size>::SquaredLength() const noexcept {
  return Dot(*this, *this);
}

template <std::size_t Size>
constexpr Float Vec<Size>::Length() const noexcept {
  return Sqrt(SquaredLength());
}

template <std::size_t Size>
constexpr void Vec<Size>::Normalize() MT_NOEXCEPT_RELEASE {
  const auto squared_length = SquaredLength();
  if constexpr (config::IsDebug()) {
    if (IsZero(squared_length)) [[unlikely]] {
      throw std::overflow_error("Vec::Normalize");
    }
  }

  *this *= kOne / Sqrt(squared_length);
}

template <std::size_t Size>
constexpr Vec<Size> Vec<Size>::Normalized() const MT_NOEXCEPT_RELEASE {
  auto normalized = *this;
  normalized.Normalize();
  return normalized;
}

template <std::size_t Size>
constexpr bool Vec<Size>::IsNormalized() const noexcept {
  return IsEqual(Length(), kOne);
}

using Vec2 = Vec<2>;
using Vec3 = Vec<3>;
