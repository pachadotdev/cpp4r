#pragma once

#include <algorithm>         // for min
#include <array>             // for array
#include <initializer_list>  // for initializer_list

#include "cpp4r/R.hpp"                // for SEXP, SEXPREC, Rf_all...
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/protect.hpp"          // for safe
#include "cpp4r/r_bool.hpp"           // for r_bool
#include "cpp4r/r_vector.hpp"         // for r_vector, r_vector<>::proxy
#include "cpp4r/sexp.hpp"             // for sexp

// Specializations for logicals

namespace cpp4r {

template <>
inline SEXPTYPE r_vector<r_bool>::get_sexptype() {
  return LGLSXP;
}

template <>
inline typename r_vector<r_bool>::underlying_type r_vector<r_bool>::get_elt(SEXP x,
                                                                            R_xlen_t i) {
  // NOPROTECT: likely too costly to unwind protect every elt
  return LOGICAL_ELT(x, i);
}

template <>
inline typename r_vector<r_bool>::underlying_type* r_vector<r_bool>::get_p(
    bool is_altrep, SEXP data) noexcept {
  return __builtin_expect(is_altrep, 0) ? nullptr : LOGICAL(data);
}

template <>
inline typename r_vector<r_bool>::underlying_type const* r_vector<r_bool>::get_const_p(
    bool is_altrep, SEXP data) noexcept {
  return LOGICAL_OR_NULL(data);
}

template <>
inline void r_vector<r_bool>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                         typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  LOGICAL_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<r_bool>::const_iterator::use_buf(bool is_altrep) noexcept {
  return is_altrep;
}

typedef r_vector<r_bool> logicals;

namespace writable {

template <>
inline void r_vector<r_bool>::set_elt(SEXP x, R_xlen_t i,
                                      typename r_vector::underlying_type value) {
  // NOPROTECT: Likely too costly to unwind protect every set elt
  SET_LOGICAL_ELT(x, i, value);
}

inline bool operator==(const r_vector<r_bool>::proxy& lhs, r_bool rhs) noexcept {
  return static_cast<r_bool>(lhs).operator==(rhs);
}

// Add constructor for initializer_list for the writable r_vector specialization
template <>
inline r_vector<r_bool>::r_vector(std::initializer_list<r_bool> il)
    : cpp4r::r_vector<r_bool>(safe[Rf_allocVector](LGLSXP, il.size())),
      capacity_(il.size()) {
  auto it = il.begin();

  if (data_p_ != nullptr) {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      data_p_[i] = static_cast<underlying_type>(*it);
    }
  } else {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      set_elt(data_, i, static_cast<underlying_type>(*it));
    }
  }
}

typedef r_vector<r_bool> logicals;

}  // namespace writable

// Forward declarations for conversion functions
typedef r_vector<int> integers;
typedef r_vector<double> doubles;

// Optimized as_logicals conversion function
inline logicals as_logicals(SEXP x) {
  SEXPTYPE x_type = detail::r_typeof(x);
  if (__builtin_expect(x_type == LGLSXP, 1)) {
    return logicals(x);
  }

  // Get length once and check for early exit
  R_xlen_t len = Rf_length(x);
  if (__builtin_expect(len == 0, 0)) {
    return writable::logicals(static_cast<R_xlen_t>(0));
  }

  if (__builtin_expect(x_type == INTSXP, 0)) {
    integers xn(x);
    writable::logicals ret(len);

    // Use direct memory access when possible for better performance
    const int* src_ptr = INTEGER_OR_NULL(x);
    int* dest_ptr = LOGICAL(ret.data());

    if (__builtin_expect(src_ptr != nullptr && dest_ptr != nullptr, 1)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (__builtin_expect(src_ptr[i] == NA_INTEGER, 0)) {
          dest_ptr[i] = NA_LOGICAL;
        } else {
          dest_ptr[i] = (src_ptr[i] != 0) ? TRUE : FALSE;
        }
      }
    } else {
      // Fallback to iterator-based approach
      std::transform(xn.begin(), xn.end(), ret.begin(), [](int value) {
        return value == NA_INTEGER ? r_bool(NA_LOGICAL) : r_bool(value != 0);
      });
    }
    return ret;
  } else if (__builtin_expect(x_type == REALSXP, 0)) {
    doubles xn(x);
    writable::logicals ret(len);

    // Use direct memory access when possible
    const double* src_ptr = REAL_OR_NULL(x);
    int* dest_ptr = LOGICAL(ret.data());

    if (__builtin_expect(src_ptr != nullptr && dest_ptr != nullptr, 1)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (__builtin_expect(ISNA(src_ptr[i]), 0)) {
          dest_ptr[i] = NA_LOGICAL;
        } else {
          dest_ptr[i] = (src_ptr[i] != 0.0) ? TRUE : FALSE;
        }
      }
    } else {
      // Fallback to iterator-based approach
      std::transform(xn.begin(), xn.end(), ret.begin(), [](double value) {
        return ISNA(value) ? r_bool(NA_LOGICAL) : r_bool(value != 0.0);
      });
    }
    return ret;
  }

  throw type_error(LGLSXP, x_type);
}

// Optimized comparison operators for r_vector<r_bool>
template <>
inline bool operator==(const r_vector<r_bool>& lhs, const r_vector<r_bool>& rhs) {
  if (lhs.size() != rhs.size()) return false;

  // Fast path: if both vectors point to the same data, they're equal
  if (lhs.data() == rhs.data()) return true;

  // Use direct memory comparison when possible
  const int* lhs_ptr = LOGICAL_OR_NULL(lhs.data());
  const int* rhs_ptr = LOGICAL_OR_NULL(rhs.data());

  if (__builtin_expect(lhs_ptr != nullptr && rhs_ptr != nullptr, 1)) {
    R_xlen_t len = lhs.size();
    for (R_xlen_t i = 0; i < len; ++i) {
      if (__builtin_expect(lhs_ptr[i] != rhs_ptr[i], 0)) return false;
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
inline bool operator!=(const r_vector<r_bool>& lhs, const r_vector<r_bool>& rhs) {
  return !(lhs == rhs);
}

// Optimized logical operations for boolean vectors
inline writable::logicals logical_and(const r_vector<r_bool>& lhs,
                                      const r_vector<r_bool>& rhs) {
  R_xlen_t len = lhs.size();
  if (len != rhs.size()) {
    throw std::invalid_argument("Vector lengths must be equal for logical AND");
  }

  writable::logicals result(len);

  const int* lhs_ptr = LOGICAL_OR_NULL(lhs.data());
  const int* rhs_ptr = LOGICAL_OR_NULL(rhs.data());
  int* result_ptr = LOGICAL(result.data());

  if (__builtin_expect(lhs_ptr != nullptr && rhs_ptr != nullptr && result_ptr != nullptr,
                       1)) {
    // Direct memory access - vectorized operation
    for (R_xlen_t i = 0; i < len; ++i) {
      if (__builtin_expect(lhs_ptr[i] == NA_LOGICAL || rhs_ptr[i] == NA_LOGICAL, 0)) {
        result_ptr[i] = NA_LOGICAL;
      } else {
        result_ptr[i] = (lhs_ptr[i] && rhs_ptr[i]) ? TRUE : FALSE;
      }
    }
  } else {
    // Fallback to iterator approach
    auto lhs_it = lhs.cbegin();
    auto rhs_it = rhs.cbegin();
    auto result_it = result.begin();

    for (R_xlen_t i = 0; i < len; ++i, ++lhs_it, ++rhs_it, ++result_it) {
      r_bool lhs_val = *lhs_it;
      r_bool rhs_val = *rhs_it;
      if (lhs_val == NA_LOGICAL || rhs_val == NA_LOGICAL) {
        *result_it = r_bool(NA_LOGICAL);
      } else {
        *result_it = r_bool(static_cast<bool>(lhs_val) && static_cast<bool>(rhs_val));
      }
    }
  }

  return result;
}

inline writable::logicals logical_or(const r_vector<r_bool>& lhs,
                                     const r_vector<r_bool>& rhs) {
  R_xlen_t len = lhs.size();
  if (len != rhs.size()) {
    throw std::invalid_argument("Vector lengths must be equal for logical OR");
  }

  writable::logicals result(len);

  const int* lhs_ptr = LOGICAL_OR_NULL(lhs.data());
  const int* rhs_ptr = LOGICAL_OR_NULL(rhs.data());
  int* result_ptr = LOGICAL(result.data());

  if (__builtin_expect(lhs_ptr != nullptr && rhs_ptr != nullptr && result_ptr != nullptr,
                       1)) {
    // Direct memory access - vectorized operation
    for (R_xlen_t i = 0; i < len; ++i) {
      if (__builtin_expect(lhs_ptr[i] == NA_LOGICAL || rhs_ptr[i] == NA_LOGICAL, 0)) {
        result_ptr[i] = NA_LOGICAL;
      } else {
        result_ptr[i] = (lhs_ptr[i] || rhs_ptr[i]) ? TRUE : FALSE;
      }
    }
  } else {
    // Fallback to iterator approach
    auto lhs_it = lhs.cbegin();
    auto rhs_it = rhs.cbegin();
    auto result_it = result.begin();

    for (R_xlen_t i = 0; i < len; ++i, ++lhs_it, ++rhs_it, ++result_it) {
      r_bool lhs_val = *lhs_it;
      r_bool rhs_val = *rhs_it;
      if (lhs_val == NA_LOGICAL || rhs_val == NA_LOGICAL) {
        *result_it = r_bool(NA_LOGICAL);
      } else {
        *result_it = r_bool(static_cast<bool>(lhs_val) || static_cast<bool>(rhs_val));
      }
    }
  }

  return result;
}

inline writable::logicals logical_not(const r_vector<r_bool>& vec) {
  R_xlen_t len = vec.size();
  writable::logicals result(len);

  const int* src_ptr = LOGICAL_OR_NULL(vec.data());
  int* result_ptr = LOGICAL(result.data());

  if (__builtin_expect(src_ptr != nullptr && result_ptr != nullptr, 1)) {
    // Direct memory access - vectorized operation
    for (R_xlen_t i = 0; i < len; ++i) {
      if (__builtin_expect(src_ptr[i] == NA_LOGICAL, 0)) {
        result_ptr[i] = NA_LOGICAL;
      } else {
        result_ptr[i] = src_ptr[i] ? FALSE : TRUE;
      }
    }
  } else {
    // Fallback to iterator approach
    auto src_it = vec.cbegin();
    auto result_it = result.begin();

    for (R_xlen_t i = 0; i < len; ++i, ++src_it, ++result_it) {
      r_bool val = *src_it;
      if (val == NA_LOGICAL) {
        *result_it = r_bool(NA_LOGICAL);
      } else {
        *result_it = r_bool(!static_cast<bool>(val));
      }
    }
  }

  return result;
}

}  // namespace cpp4r
