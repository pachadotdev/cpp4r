#pragma once

#include <algorithm>         // for min
#include <array>             // for array
#include <initializer_list>  // for initializer_list

#include "R_ext/Arith.h"              // for NA_INTEGER
#include "cpp4r/R.hpp"                // for SEXP, SEXPREC, Rf_allocVector
#include "cpp4r/as.hpp"               // for as_sexp
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/cpp_version.hpp"      // for CPP4R feature detection
#include "cpp4r/protect.hpp"          // for safe
#include "cpp4r/r_bool.hpp"           // for r_bool
#include "cpp4r/r_vector.hpp"         // for r_vector, r_vector<>::proxy
#include "cpp4r/sexp.hpp"             // for sexp

// Specializations for integers

namespace cpp4r {

template <>
inline SEXPTYPE r_vector<int>::get_sexptype() {
  return INTSXP;
}

template <>
inline typename r_vector<int>::underlying_type r_vector<int>::get_elt(SEXP x,
                                                                      R_xlen_t i) {
  // NOPROTECT: likely too costly to unwind protect every elt
  return INTEGER_ELT(x, i);
}

template <>
inline typename r_vector<int>::underlying_type* r_vector<int>::get_p(bool is_altrep,
                                                                     SEXP data) {
  return INTEGER(data);
}

template <>
inline typename r_vector<int>::underlying_type const* r_vector<int>::get_const_p(
    bool is_altrep, SEXP data) {
  return INTEGER(data);
}

template <>
inline void r_vector<int>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                      typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  INTEGER_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<int>::generic_const_iterator::use_buf(bool is_altrep) {
  return is_altrep;
}

typedef r_vector<int> integers;

namespace writable {

template <>
inline void r_vector<int>::set_elt(SEXP x, R_xlen_t i,
                                   typename r_vector::underlying_type value) {
  // NOPROTECT: Likely too costly to unwind protect every set elt
  SET_INTEGER_ELT(x, i, value);
}

typedef r_vector<int> integers;

}  // namespace writable

template <>
inline int na() {
  return NA_INTEGER;
}

// forward declaration

typedef r_vector<double> doubles;
typedef r_vector<r_bool> logicals;

inline integers as_integers(SEXP x) {
  if (detail::r_typeof(x) == INTSXP) {
    return integers(x);
  } else if (detail::r_typeof(x) == REALSXP) {
    doubles xn(x);
    size_t len = xn.size();
    writable::integers ret(len);

#if CPP4R_HAS_CXX20
    // C++20: Use [[likely]] attribute and direct pointer access for best performance
    const double* CPP4R_RESTRICT x_ptr = REAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

    for (size_t i = 0; i < len; ++i) {
      double value = x_ptr[i];
      if (ISNA(value)) {
        ret_ptr[i] = NA_INTEGER;
      } else if (!is_convertible_without_loss_to_integer(value)) [[unlikely]] {
        throw std::runtime_error("All elements must be integer-like");
      } else {
        ret_ptr[i] = static_cast<int>(value);
      }
    }
#elif CPP4R_HAS_CXX17
    // C++17: Use if constexpr and direct pointers for good performance
    const double* CPP4R_RESTRICT x_ptr = REAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

    for (size_t i = 0; i < len; ++i) {
      double value = x_ptr[i];
      if (ISNA(value)) {
        ret_ptr[i] = NA_INTEGER;
      } else if (CPP4R_UNLIKELY(!is_convertible_without_loss_to_integer(value))) {
        throw std::runtime_error("All elements must be integer-like");
      } else {
        ret_ptr[i] = static_cast<int>(value);
      }
    }
#else
    // C++11-14: Use raw loop with direct pointers
    const double* CPP4R_RESTRICT x_ptr = REAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

    for (size_t i = 0; i < len; ++i) {
      double value = x_ptr[i];
      if (ISNA(value)) {
        ret_ptr[i] = NA_INTEGER;
      } else if (CPP4R_UNLIKELY(!is_convertible_without_loss_to_integer(value))) {
        throw std::runtime_error("All elements must be integer-like");
      } else {
        ret_ptr[i] = static_cast<int>(value);
      }
    }
#endif

    return ret;
  } else if (detail::r_typeof(x) == LGLSXP) {
    logicals xn(x);
    size_t len = xn.size();
    writable::integers ret(len);

#if CPP4R_HAS_CXX20
    // C++20: Direct pointer access with [[likely]] attribute
    const int* CPP4R_RESTRICT x_ptr = LOGICAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

    for (size_t i = 0; i < len; ++i) {
      int value = x_ptr[i];
      if (value == NA_LOGICAL) [[unlikely]] {
        ret_ptr[i] = NA_INTEGER;
      } else {
        ret_ptr[i] = static_cast<int>(value);
      }
    }
#elif CPP4R_HAS_CXX17
    // C++17: Direct pointer access with builtin hints
    const int* CPP4R_RESTRICT x_ptr = LOGICAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

    for (size_t i = 0; i < len; ++i) {
      int value = x_ptr[i];
      ret_ptr[i] =
          CPP4R_UNLIKELY(value == NA_LOGICAL) ? NA_INTEGER : static_cast<int>(value);
    }
#else
    // C++11-14: Use raw loop with direct pointers
    const int* CPP4R_RESTRICT x_ptr = LOGICAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

    for (size_t i = 0; i < len; ++i) {
      int value = x_ptr[i];
      ret_ptr[i] =
          CPP4R_UNLIKELY(value == NA_LOGICAL) ? NA_INTEGER : static_cast<int>(value);
    }
#endif

    return ret;
  }

  throw type_error(INTSXP, detail::r_typeof(x));
}

}  // namespace cpp4r

// Note: Proxy Behavior in writable::integers
//
// When using writable::integers, operator[] returns a proxy object that allows
// both reading and writing. For cases where you need the actual int value
// (e.g., when using with C-style variadic functions like Rprintf), use one of
// these three approaches:
//
// 1. Direct value access: vec.value(i)        [Recommended]
// 2. Explicit cast: (int)vec[i]
// 3. Auto with explicit type: int val = vec[i];
//
// Example demonstrating the issue and solutions:
//   writable::integers vec;
//   vec.push_back(42);
//
//   // This may print garbage due to proxy object:
//   // Rprintf("Value: %d\n", vec[0]);  // DON'T DO THIS
//
//   // These all work correctly:
//   Rprintf("Value: %d\n", vec.value(0));  // Recommended
//   Rprintf("Value: %d\n", (int)vec[0]);   // Also works
//   int val = vec[0]; Rprintf("Value: %d\n", val);  // Also works
