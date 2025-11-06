#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <algorithm>         // for min, tranform
#include <array>             // for array
#include <initializer_list>  // for initializer_list

#include "R_ext/Arith.h"       // for ISNA
#include "cpp4r/R.hpp"         // for SEXP, SEXPREC, Rf_allocVector, REAL
#include "cpp4r/as.hpp"        // for as_sexp
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
                                                                           SEXP data) noexcept {
#if CPP4R_HAS_CXX20
  if (CPP4R_UNLIKELY(is_altrep)) {
    return nullptr;
  } else {
    return REAL(data);
  }
#else
  if (is_altrep) {
    return nullptr;
  } else {
    return REAL(data);
  }
#endif
}

template <>
inline typename r_vector<double>::underlying_type const* r_vector<double>::get_const_p(
    bool is_altrep, SEXP data) noexcept {
  return REAL_OR_NULL(data);
}

template <>
inline void r_vector<double>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                         typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  REAL_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<double>::const_iterator::use_buf(bool is_altrep) noexcept {
  return __builtin_expect(is_altrep, 0);
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
  SEXPTYPE x_type = detail::r_typeof(x);
#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(x_type == REALSXP)) {
    return doubles(x);
  }

  // Get length once and check for early exit
  R_xlen_t len = Rf_length(x);
  if (CPP4R_UNLIKELY(len == 0)) {
    return writable::doubles(static_cast<R_xlen_t>(0));
  }

  if (CPP4R_UNLIKELY(x_type == INTSXP)) {
#else
  if (__builtin_expect(x_type == REALSXP, 1)) {
    return doubles(x);
  }

  // Get length once and check for early exit
  R_xlen_t len = Rf_length(x);
  if (__builtin_expect(len == 0, 0)) {
    return writable::doubles(static_cast<R_xlen_t>(0));
  }

  if (__builtin_expect(x_type == INTSXP, 0)) {
#endif
    integers xn(x);
    writable::doubles ret(len);

    // For large vectors, check if we can use direct pointer access for better performance
    const int* src_ptr = INTEGER_OR_NULL(x);
    double* dest_ptr = REAL(ret.data());

#if CPP4R_HAS_CXX20
    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
#else
    if (__builtin_expect(src_ptr != nullptr && dest_ptr != nullptr, 1)) {
#endif
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        dest_ptr[i] = __builtin_expect(src_ptr[i] == NA_INTEGER, 0)
                          ? NA_REAL
                          : static_cast<double>(src_ptr[i]);
      }
    } else {
      // Fallback to iterator-based approach
      std::transform(xn.begin(), xn.end(), ret.begin(), [](int value) {
        return value == NA_INTEGER ? NA_REAL : static_cast<double>(value);
      });
    }
    return ret;
#if CPP4R_HAS_CXX20
  } else if (CPP4R_UNLIKELY(x_type == LGLSXP)) {
#else
  } else if (__builtin_expect(x_type == LGLSXP, 0)) {
#endif
    logicals xn(x);
    writable::doubles ret(len);

    // For large vectors, check if we can use direct pointer access
    const int* src_ptr = LOGICAL_OR_NULL(x);
    double* dest_ptr = REAL(ret.data());

#if CPP4R_HAS_CXX20
    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
#else
    if (__builtin_expect(src_ptr != nullptr && dest_ptr != nullptr, 1)) {
#endif
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        int logical_val = src_ptr[i];
        dest_ptr[i] = __builtin_expect(logical_val == NA_LOGICAL, 0)
                          ? NA_REAL
                          : static_cast<double>(logical_val);
      }
    } else {
      // Fallback to iterator-based approach
      std::transform(xn.begin(), xn.end(), ret.begin(), [](r_bool value) {
        return value == NA_LOGICAL ? NA_REAL
                                   : static_cast<double>(static_cast<int>(value));
      });
    }
    return ret;
  }

  throw type_error(REALSXP, x_type);
}

template <>
#if CPP4R_HAS_CXX17
CPP4R_NODISCARD inline double na() {
  return NA_REAL;
}
#else
inline double na() {
  return NA_REAL;
}
#endif

// Optimized comparison operators for r_vector<double>
template <>
inline bool operator==(const r_vector<double>& lhs, const r_vector<double>& rhs) {
#if CPP4R_HAS_CXX20
  if (CPP4R_UNLIKELY(lhs.size() != rhs.size())) {
    return false;
  }

  // Fast path: if both vectors point to the same data, they're equal
  if (CPP4R_LIKELY(lhs.data() == rhs.data())) {
    return true;
  }
#else
  if (lhs.size() != rhs.size()) return false;

  // Fast path: if both vectors point to the same data, they're equal
  if (lhs.data() == rhs.data()) return true;
#endif

  // Use direct memory comparison when possible
  const double* lhs_ptr = REAL_OR_NULL(lhs.data());
  const double* rhs_ptr = REAL_OR_NULL(rhs.data());

#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(lhs_ptr != nullptr && rhs_ptr != nullptr)) {
#else
  if (__builtin_expect(lhs_ptr != nullptr && rhs_ptr != nullptr, 1)) {
#endif
    R_xlen_t len = lhs.size();
    for (R_xlen_t i = 0; i < len; ++i) {
      // Handle NaN comparison correctly - NaN != NaN should be true
#if CPP4R_HAS_CXX20
      if (CPP4R_UNLIKELY(ISNAN(lhs_ptr[i]) || ISNAN(rhs_ptr[i]))) {
        if (!(ISNAN(lhs_ptr[i]) && ISNAN(rhs_ptr[i]))) return false;
      } else if (CPP4R_UNLIKELY(lhs_ptr[i] != rhs_ptr[i])) {
        return false;
      }
#else
      if (__builtin_expect(ISNAN(lhs_ptr[i]) || ISNAN(rhs_ptr[i]), 0)) {
        if (!(ISNAN(lhs_ptr[i]) && ISNAN(rhs_ptr[i]))) return false;
      } else if (__builtin_expect(lhs_ptr[i] != rhs_ptr[i], 0)) {
        return false;
      }
#endif
    }
    return true;
  } else {
    // Fallback to iterator comparison
    auto lhs_it = lhs.cbegin();
    auto rhs_it = rhs.cbegin();
    auto lhs_end = lhs.cend();

    for (; lhs_it != lhs_end; ++lhs_it, ++rhs_it) {
      if (!(*lhs_it == *rhs_it)) return false;
    }
    return true;
  }
}

template <>
inline bool operator!=(const r_vector<double>& lhs, const r_vector<double>& rhs) {
  return !(lhs == rhs);
}

}  // namespace cpp4r
