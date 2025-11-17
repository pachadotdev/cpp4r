#pragma once

#include <algorithm>         // for min, tranform
#include <array>             // for array
#include <initializer_list>  // for initializer_list

#include "R_ext/Arith.h"       // for ISNA
#include "cpp4r/R.hpp"         // for SEXP, SEXPREC, Rf_allocVector, REAL
#include "cpp4r/as.hpp"        // for as_sexp
#include "cpp4r/cpp_version.hpp"  // for CPP4R feature detection
#include "cpp4r/protect.hpp"   // for safe
#include "cpp4r/r_bool.hpp"    // for r_bool
#include "cpp4r/r_vector.hpp"  // for vector, vector<>::proxy, vector<>::...
#include "cpp4r/sexp.hpp"      // for sexp

// Specializations for doubles

namespace cpp4r {

template <>
inline SEXPTYPE r_vector<double>::get_sexptype() {
  return REALSXP;
}

template <>
inline typename r_vector<double>::underlying_type r_vector<double>::get_elt(SEXP x,
                                                                            R_xlen_t i) {
  // NOPROTECT: likely too costly to unwind protect every elt
  return REAL_ELT(x, i);
}

template <>
inline typename r_vector<double>::underlying_type* r_vector<double>::get_p(bool is_altrep,
                                                                           SEXP data) {
  if (is_altrep) {
    return nullptr;
  } else {
    return REAL(data);
  }
}

template <>
inline typename r_vector<double>::underlying_type const* r_vector<double>::get_const_p(
    bool is_altrep, SEXP data) {
  return REAL_OR_NULL(data);
}

template <>
inline void r_vector<double>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                         typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  REAL_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<double>::const_iterator::use_buf(bool is_altrep) {
  return is_altrep;
}

typedef r_vector<double> doubles;

namespace writable {

template <>
inline void r_vector<double>::set_elt(SEXP x, R_xlen_t i,
                                      typename r_vector::underlying_type value) {
  // NOPROTECT: Likely too costly to unwind protect every set elt
  SET_REAL_ELT(x, i, value);
}

typedef r_vector<double> doubles;

}  // namespace writable

typedef r_vector<int> integers;
typedef r_vector<r_bool> logicals;

inline doubles as_doubles(SEXP x) {
  if (detail::r_typeof(x) == REALSXP) {
    return doubles(x);
  } else if (detail::r_typeof(x) == INTSXP) {
    integers xn(x);
    size_t len = xn.size();
    writable::doubles ret(len);
    
    const int* CPP4R_RESTRICT x_ptr = INTEGER(xn.data());
    double* CPP4R_RESTRICT ret_ptr = REAL(ret.data());
    
    // Optimized loop with branch prediction hints
    for (size_t i = 0; i < len; ++i) {
      int val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(val != NA_INTEGER) ? static_cast<double>(val) : NA_REAL;
    }
    
    return ret;
  } else if (detail::r_typeof(x) == LGLSXP) {
    logicals xn(x);
    size_t len = xn.size();
    writable::doubles ret(len);
    
    const int* CPP4R_RESTRICT x_ptr = LOGICAL(xn.data());
    double* CPP4R_RESTRICT ret_ptr = REAL(ret.data());
    
    // Optimized loop with branch prediction hints
    for (size_t i = 0; i < len; ++i) {
      int val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(val != NA_LOGICAL) ? static_cast<double>(val) : NA_REAL;
    }
    
    return ret;
  }

  throw type_error(REALSXP, detail::r_typeof(x));
}

template <>
inline double na() {
  return NA_REAL;
}

}  // namespace cpp4r
