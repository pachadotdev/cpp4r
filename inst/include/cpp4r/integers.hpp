#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <algorithm>         // for min
#include <array>             // for array
#include <initializer_list>  // for initializer_list

#include "R_ext/Arith.h"              // for NA_INTEGER
#include "cpp4r/R.hpp"                // for SEXP, SEXPREC, Rf_allocVector
#include "cpp4r/as.hpp"               // for as_sexp
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
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
  if (is_altrep) {
    return nullptr;
  } else {
    return INTEGER(data);
  }
}

template <>
inline typename r_vector<int>::underlying_type const* r_vector<int>::get_const_p(
    bool is_altrep, SEXP data) {
  return INTEGER_OR_NULL(data);
}template <>
inline void r_vector<int>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                      typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  INTEGER_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<int>::const_iterator::use_buf(bool is_altrep) {
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

// No special conversions needed for int - it's the native type

typedef r_vector<int> integers;

// Add constructor for initializer_list for the writable r_vector specialization
template <>
inline r_vector<int>::r_vector(std::initializer_list<int> il)
    : cpp4r::r_vector<int>(safe[Rf_allocVector](INTSXP, il.size())),
      capacity_(il.size()) {
  auto it = il.begin();

  if (data_p_ != nullptr) {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      data_p_[i] = *it;
    }
  } else {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      set_elt(data_, i, *it);
    }
  }
}

}  // namespace writable

template <>
#if CPP4R_HAS_CXX17
CPP4R_NODISCARD inline int na() {
  return NA_INTEGER;
}
#else
inline int na() {
  return NA_INTEGER;
}
#endif

// forward declaration

typedef r_vector<double> doubles;
typedef r_vector<r_bool> logicals;

inline integers as_integers(SEXP x) {
  SEXPTYPE x_type = detail::r_typeof(x);
  if (x_type == INTSXP) {
    return integers(x);
  }

  if (x_type == REALSXP) {
    doubles xn(x);
    writable::integers ret(xn.size());
    // Trust the compiler to optimize std::transform
    std::transform(xn.begin(), xn.end(), ret.begin(), [](double value) {
      if (ISNA(value)) {
        return NA_INTEGER;
      }
      if (!is_convertible_without_loss_to_integer(value)) {
        throw std::runtime_error("All elements must be integer-like");
      }
      return static_cast<int>(value);
    });
    return ret;
  }

  if (x_type == LGLSXP) {
    logicals xn(x);
    writable::integers ret(xn.size());
    std::transform(xn.begin(), xn.end(), ret.begin(), [](r_bool value) {
      return value == NA_LOGICAL ? NA_INTEGER : static_cast<int>(static_cast<int>(value));
    });
    return ret;
  }

  throw type_error(INTSXP, x_type);
}

}  // namespace cpp4r
