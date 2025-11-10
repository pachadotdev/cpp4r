#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <limits>  // for numeric_limits
#include <ostream>
#include <type_traits>  // for is_convertible, enable_if

#include "R_ext/Boolean.h"    // for Rboolean
#include "cpp4r/R.hpp"        // for SEXP, SEXPREC, ...
#include "cpp4r/as.hpp"       // for as_sexp
#include "cpp4r/protect.hpp"  // for unwind_protect
#include "cpp4r/r_vector.hpp"
#include "cpp4r/sexp.hpp"  // for sexp

namespace cpp4r {

class r_bool {
 public:
  r_bool() = default;

  r_bool(SEXP data) {
    if (Rf_isLogical(data) && Rf_xlength(data) == 1) {
      value_ = static_cast<Rboolean>(LOGICAL_ELT(data, 0));
      return;
    }
    throw std::invalid_argument("Invalid r_bool value");
  }

  constexpr r_bool(bool value) noexcept : value_(value ? TRUE : FALSE) {}
  constexpr r_bool(Rboolean value) noexcept : value_(value) {}
  constexpr r_bool(int value) noexcept : value_(from_int(value)) {}

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD constexpr operator bool() const noexcept { return value_ == TRUE; }
  CPP4R_NODISCARD constexpr operator int() const noexcept { return value_; }
  CPP4R_NODISCARD constexpr operator Rboolean() const noexcept {
    return value_ ? TRUE : FALSE;
  }
#else
  constexpr operator bool() const noexcept { return value_ == TRUE; }
  constexpr operator int() const noexcept { return value_; }
  constexpr operator Rboolean() const noexcept { return value_ ? TRUE : FALSE; }
#endif

  constexpr bool operator==(r_bool rhs) const noexcept { return value_ == rhs.value_; }
  constexpr bool operator==(bool rhs) const noexcept { return operator==(r_bool(rhs)); }
  constexpr bool operator==(Rboolean rhs) const noexcept {
    return operator==(r_bool(rhs));
  }
  constexpr bool operator==(int rhs) const noexcept { return operator==(r_bool(rhs)); }

  constexpr bool operator!=(r_bool rhs) const noexcept { return !operator==(rhs); }
  constexpr bool operator!=(bool rhs) const noexcept { return !operator==(rhs); }
  constexpr bool operator!=(Rboolean rhs) const noexcept { return !operator==(rhs); }
  constexpr bool operator!=(int rhs) const noexcept { return !operator==(rhs); }

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD constexpr bool is_na() const noexcept { return value_ == na; }
#else
  constexpr bool is_na() const noexcept { return value_ == na; }
#endif

 private:
  static constexpr int na = std::numeric_limits<int>::min();

  static constexpr int from_int(int value) noexcept {
    return (value == static_cast<int>(FALSE)) ? FALSE
           : (value == static_cast<int>(na))  ? na
                                              : TRUE;
  }

  int value_ = na;
};

inline std::ostream& operator<<(std::ostream& os, r_bool const& value) {
  if (value.is_na()) {
    os << "NA";
  } else {
    os << (static_cast<bool>(value) ? "TRUE" : "FALSE");
  }
  return os;
}

template <typename T, typename R = void>
using enable_if_r_bool = enable_if_t<std::is_same<T, r_bool>::value, R>;

template <typename T>
enable_if_r_bool<T, SEXP> as_sexp(T from) {
  sexp res = Rf_allocVector(LGLSXP, 1);
  unwind_protect([&] { SET_LOGICAL_ELT(res.data(), 0, from); });
  return res;
}

template <>
#if CPP4R_HAS_CXX17
CPP4R_NODISCARD inline r_bool na() {
  return NA_LOGICAL;
}
#else
inline r_bool na() {
  return NA_LOGICAL;
}
#endif

namespace traits {
template <>
struct get_underlying_type<r_bool> {
  using type = int;
};
}  // namespace traits

}  // namespace cpp4r
