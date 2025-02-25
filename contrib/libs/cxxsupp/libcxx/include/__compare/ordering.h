//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___COMPARE_ORDERING_H
#define _LIBCPP___COMPARE_ORDERING_H

#include <__config>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

#if defined(_MSC_VER) && !defined(__clang__)
// Omit inline namespace __y1, as MSVC2019 fails to find ordering classes
// inside the inlined namespace when compiling the <=> operators.
// See CROWDFUNDING-6 for the details.
namespace std {
#else
_LIBCPP_BEGIN_NAMESPACE_STD
#endif

#if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_SPACESHIP_OPERATOR)

// exposition only
enum class _LIBCPP_ENUM_VIS _EqResult : unsigned char {
  __zero = 0,
  __equal = __zero,
  __equiv = __equal,
  __nonequal = 1,
  __nonequiv = __nonequal
};

enum class _LIBCPP_ENUM_VIS _OrdResult : signed char {
  __less = -1,
  __greater = 1
};

enum class _LIBCPP_ENUM_VIS _NCmpResult : signed char {
  __unordered = -127
};

class partial_ordering;
class weak_ordering;
class strong_ordering;

template<class _Tp, class... _Args>
inline constexpr bool __one_of_v = (is_same_v<_Tp, _Args> || ...);

struct _CmpUnspecifiedParam {
  _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEVAL
  _CmpUnspecifiedParam(int _CmpUnspecifiedParam::*) noexcept {}

  template<class _Tp, class = enable_if_t<!__one_of_v<_Tp, int, partial_ordering, weak_ordering, strong_ordering>>>
  _CmpUnspecifiedParam(_Tp) = delete;
};

class partial_ordering {
  using _ValueT = signed char;

  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr partial_ordering(_EqResult __v) noexcept
      : __value_(_ValueT(__v)) {}

  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr partial_ordering(_OrdResult __v) noexcept
      : __value_(_ValueT(__v)) {}

  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr partial_ordering(_NCmpResult __v) noexcept
      : __value_(_ValueT(__v)) {}

  constexpr bool __is_ordered() const noexcept {
    return __value_ != _ValueT(_NCmpResult::__unordered);
  }
public:
  // valid values
  static const partial_ordering less;
  static const partial_ordering equivalent;
  static const partial_ordering greater;
  static const partial_ordering unordered;

  // comparisons
  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator==(partial_ordering, partial_ordering) noexcept = default;

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator==(partial_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__is_ordered() && __v.__value_ == 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator< (partial_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__is_ordered() && __v.__value_ < 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator<=(partial_ordering __v, _CmpUnspecifiedParam) noexcept  {
    return __v.__is_ordered() && __v.__value_ <= 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator> (partial_ordering __v, _CmpUnspecifiedParam) noexcept  {
    return __v.__is_ordered() && __v.__value_ > 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator>=(partial_ordering __v, _CmpUnspecifiedParam) noexcept  {
    return __v.__is_ordered() && __v.__value_ >= 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator< (_CmpUnspecifiedParam, partial_ordering __v) noexcept  {
    return __v.__is_ordered() && 0 < __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator<=(_CmpUnspecifiedParam, partial_ordering __v) noexcept  {
    return __v.__is_ordered() && 0 <= __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator> (_CmpUnspecifiedParam, partial_ordering __v) noexcept  {
    return __v.__is_ordered() && 0 > __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator>=(_CmpUnspecifiedParam, partial_ordering __v) noexcept  {
    return __v.__is_ordered() && 0 >= __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr partial_ordering operator<=>(partial_ordering __v, _CmpUnspecifiedParam) noexcept  {
    return __v;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr partial_ordering operator<=>(_CmpUnspecifiedParam, partial_ordering __v) noexcept  {
    return __v < 0 ? partial_ordering::greater : (__v > 0 ? partial_ordering::less : __v);
  }
private:
  _ValueT __value_;
};

_LIBCPP_INLINE_VAR constexpr partial_ordering partial_ordering::less(_OrdResult::__less);
_LIBCPP_INLINE_VAR constexpr partial_ordering partial_ordering::equivalent(_EqResult::__equiv);
_LIBCPP_INLINE_VAR constexpr partial_ordering partial_ordering::greater(_OrdResult::__greater);
_LIBCPP_INLINE_VAR constexpr partial_ordering partial_ordering::unordered(_NCmpResult ::__unordered);

class weak_ordering {
  using _ValueT = signed char;

  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr weak_ordering(_EqResult __v) noexcept : __value_(_ValueT(__v)) {}
  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr weak_ordering(_OrdResult __v) noexcept : __value_(_ValueT(__v)) {}

public:
  static const weak_ordering less;
  static const weak_ordering equivalent;
  static const weak_ordering greater;

  _LIBCPP_INLINE_VISIBILITY
  constexpr operator partial_ordering() const noexcept {
    return __value_ == 0 ? partial_ordering::equivalent
        : (__value_ < 0 ? partial_ordering::less : partial_ordering::greater);
  }

  // comparisons
  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator==(weak_ordering, weak_ordering) noexcept = default;

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator==(weak_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ == 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator< (weak_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ < 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator<=(weak_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ <= 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator> (weak_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ > 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator>=(weak_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ >= 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator< (_CmpUnspecifiedParam, weak_ordering __v) noexcept {
    return 0 < __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator<=(_CmpUnspecifiedParam, weak_ordering __v) noexcept {
    return 0 <= __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator> (_CmpUnspecifiedParam, weak_ordering __v) noexcept {
    return 0 > __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator>=(_CmpUnspecifiedParam, weak_ordering __v) noexcept {
    return 0 >= __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr weak_ordering operator<=>(weak_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr weak_ordering operator<=>(_CmpUnspecifiedParam, weak_ordering __v) noexcept {
    return __v < 0 ? weak_ordering::greater : (__v > 0 ? weak_ordering::less : __v);
  }

private:
  _ValueT __value_;
};

_LIBCPP_INLINE_VAR constexpr weak_ordering weak_ordering::less(_OrdResult::__less);
_LIBCPP_INLINE_VAR constexpr weak_ordering weak_ordering::equivalent(_EqResult::__equiv);
_LIBCPP_INLINE_VAR constexpr weak_ordering weak_ordering::greater(_OrdResult::__greater);
class strong_ordering {
  using _ValueT = signed char;

  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr strong_ordering(_EqResult __v) noexcept : __value_(_ValueT(__v)) {}
  _LIBCPP_INLINE_VISIBILITY
  explicit constexpr strong_ordering(_OrdResult __v) noexcept : __value_(_ValueT(__v)) {}

public:
  static const strong_ordering less;
  static const strong_ordering equal;
  static const strong_ordering equivalent;
  static const strong_ordering greater;

  // conversions
  _LIBCPP_INLINE_VISIBILITY
  constexpr operator partial_ordering() const noexcept {
    return __value_ == 0 ? partial_ordering::equivalent
        : (__value_ < 0 ? partial_ordering::less : partial_ordering::greater);
  }

  _LIBCPP_INLINE_VISIBILITY
  constexpr operator weak_ordering() const noexcept {
    return __value_ == 0 ? weak_ordering::equivalent
        : (__value_ < 0 ? weak_ordering::less : weak_ordering::greater);
  }

  // comparisons
  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator==(strong_ordering, strong_ordering) noexcept = default;

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator==(strong_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ == 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator< (strong_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ < 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator<=(strong_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ <= 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator> (strong_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ > 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator>=(strong_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v.__value_ >= 0;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator< (_CmpUnspecifiedParam, strong_ordering __v) noexcept {
    return 0 < __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator<=(_CmpUnspecifiedParam, strong_ordering __v) noexcept {
    return 0 <= __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator> (_CmpUnspecifiedParam, strong_ordering __v) noexcept {
    return 0 > __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr bool operator>=(_CmpUnspecifiedParam, strong_ordering __v) noexcept {
    return 0 >= __v.__value_;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr strong_ordering operator<=>(strong_ordering __v, _CmpUnspecifiedParam) noexcept {
    return __v;
  }

  _LIBCPP_INLINE_VISIBILITY friend constexpr strong_ordering operator<=>(_CmpUnspecifiedParam, strong_ordering __v) noexcept {
    return __v < 0 ? strong_ordering::greater : (__v > 0 ? strong_ordering::less : __v);
  }

private:
  _ValueT __value_;
};

_LIBCPP_INLINE_VAR constexpr strong_ordering strong_ordering::less(_OrdResult::__less);
_LIBCPP_INLINE_VAR constexpr strong_ordering strong_ordering::equal(_EqResult::__equal);
_LIBCPP_INLINE_VAR constexpr strong_ordering strong_ordering::equivalent(_EqResult::__equiv);
_LIBCPP_INLINE_VAR constexpr strong_ordering strong_ordering::greater(_OrdResult::__greater);

// named comparison functions
_LIBCPP_INLINE_VISIBILITY
constexpr bool is_eq(partial_ordering __cmp) noexcept    { return __cmp == 0; }

_LIBCPP_INLINE_VISIBILITY
constexpr bool is_neq(partial_ordering __cmp) noexcept    { return __cmp != 0; }

_LIBCPP_INLINE_VISIBILITY
constexpr bool is_lt(partial_ordering __cmp) noexcept { return __cmp < 0; }

_LIBCPP_INLINE_VISIBILITY
constexpr bool is_lteq(partial_ordering __cmp) noexcept { return __cmp <= 0; }

_LIBCPP_INLINE_VISIBILITY
constexpr bool is_gt(partial_ordering __cmp) noexcept { return __cmp > 0; }

_LIBCPP_INLINE_VISIBILITY
constexpr bool is_gteq(partial_ordering __cmp) noexcept { return __cmp >= 0; }

#endif // _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_SPACESHIP_OPERATOR)

#if defined(_MSC_VER) && !defined(__clang__)
}
#else
_LIBCPP_END_NAMESPACE_STD
#endif

#endif // _LIBCPP___COMPARE_ORDERING_H
