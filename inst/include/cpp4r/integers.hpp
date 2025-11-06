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
                                                                     SEXP data) noexcept {
#if CPP4R_HAS_CXX20
  return CPP4R_UNLIKELY(is_altrep) ? nullptr : INTEGER(data);
#else
  return __builtin_expect(is_altrep, 0) ? nullptr : INTEGER(data);
#endif
}

template <>
inline typename r_vector<int>::underlying_type const* r_vector<int>::get_const_p(
    bool is_altrep, SEXP data) noexcept {
  return INTEGER_OR_NULL(data);
}

template <>
inline void r_vector<int>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                      typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  INTEGER_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<int>::const_iterator::use_buf(bool is_altrep) noexcept {
#if CPP4R_HAS_CXX20
  return CPP4R_UNLIKELY(__builtin_expect(is_altrep, 0));
#else
  return __builtin_expect(is_altrep, 0);
#endif
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

#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(data_p_ != nullptr)) {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      data_p_[i] = *it;
    }
  } else {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      set_elt(data_, i, *it);
    }
  }
#else
  if (data_p_ != nullptr) {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      data_p_[i] = *it;
    }
  } else {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      set_elt(data_, i, *it);
    }
  }
#endif
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
#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(x_type == INTSXP)) {
    return integers(x);
  }
#else
  if (__builtin_expect(x_type == INTSXP, 1)) {
    return integers(x);
  }
#endif

  // Get length once and check for early exit
  R_xlen_t len = Rf_length(x);
#if CPP4R_HAS_CXX20
  if (CPP4R_UNLIKELY(len == 0)) {
    return writable::integers(static_cast<R_xlen_t>(0));
  }
#else
  if (__builtin_expect(len == 0, 0)) {
    return writable::integers(static_cast<R_xlen_t>(0));
  }
#endif

  if (__builtin_expect(x_type == REALSXP, 0)) {
    doubles xn(x);
    writable::integers ret(len);

    // Use direct memory access when possible for better performance
    const double* src_ptr = REAL_OR_NULL(x);
    int* dest_ptr = INTEGER(ret.data());

#if CPP4R_HAS_CXX20
    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(ISNA(src_ptr[i]))) {
          dest_ptr[i] = NA_INTEGER;
        } else if (__builtin_expect(!is_convertible_without_loss_to_integer(src_ptr[i]),
                                    0)) {
          throw std::runtime_error("All elements must be integer-like");
        } else {
          dest_ptr[i] = static_cast<int>(src_ptr[i]);
        }
      }
    } else {
#else
    if (__builtin_expect(src_ptr != nullptr && dest_ptr != nullptr, 1)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (__builtin_expect(ISNA(src_ptr[i]), 0)) {
          dest_ptr[i] = NA_INTEGER;
        } else if (__builtin_expect(!is_convertible_without_loss_to_integer(src_ptr[i]),
                                    0)) {
          throw std::runtime_error("All elements must be integer-like");
        } else {
          dest_ptr[i] = static_cast<int>(src_ptr[i]);
        }
      }
    } else {
#endif
      // Fallback to iterator-based approach
      std::transform(xn.begin(), xn.end(), ret.begin(), [](double value) {
        if (ISNA(value)) {
          return NA_INTEGER;
        }
        if (!is_convertible_without_loss_to_integer(value)) {
          throw std::runtime_error("All elements must be integer-like");
        }
        return static_cast<int>(value);
      });
    }
    return ret;
  } else if (__builtin_expect(x_type == LGLSXP, 0)) {
    logicals xn(x);
    writable::integers ret(len);

    // Use direct memory access when possible
    const int* src_ptr = LOGICAL_OR_NULL(x);
    int* dest_ptr = INTEGER(ret.data());

#if CPP4R_HAS_CXX20
    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        dest_ptr[i] = (src_ptr[i] == NA_LOGICAL) ? NA_INTEGER : src_ptr[i];
      }
    } else {
#else
    if (__builtin_expect(src_ptr != nullptr && dest_ptr != nullptr, 1)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        dest_ptr[i] =
            __builtin_expect(src_ptr[i] == NA_LOGICAL, 0) ? NA_INTEGER : src_ptr[i];
      }
    } else {
#endif
      // Fallback to iterator-based approach
      std::transform(xn.begin(), xn.end(), ret.begin(), [](r_bool value) {
        return value == NA_LOGICAL ? NA_INTEGER
                                   : static_cast<int>(static_cast<int>(value));
      });
    }
    return ret;
  }

  throw type_error(INTSXP, x_type);
}

// Optimized comparison operators for r_vector<int>
template <>
inline bool operator==(const r_vector<int>& lhs, const r_vector<int>& rhs) {
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
  const int* lhs_ptr = INTEGER_OR_NULL(lhs.data());
  const int* rhs_ptr = INTEGER_OR_NULL(rhs.data());

#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(lhs_ptr != nullptr && rhs_ptr != nullptr)) {
    R_xlen_t len = lhs.size();
    for (R_xlen_t i = 0; i < len; ++i) {
      if (CPP4R_UNLIKELY(lhs_ptr[i] != rhs_ptr[i])) {
        return false;
      }
    }
    return true;
  } else {
#else
  if (__builtin_expect(lhs_ptr != nullptr && rhs_ptr != nullptr, 1)) {
    R_xlen_t len = lhs.size();
    for (R_xlen_t i = 0; i < len; ++i) {
      if (__builtin_expect(lhs_ptr[i] != rhs_ptr[i], 0)) return false;
    }
    return true;
  } else {
#endif
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
inline bool operator!=(const r_vector<int>& lhs, const r_vector<int>& rhs) {
  return !(lhs == rhs);
}

// Optimized sum operation for integers (commonly used)
inline long long sum_integers(const r_vector<int>& vec) noexcept {
  long long result = 0;
  const int* ptr = INTEGER_OR_NULL(vec.data());
  R_xlen_t len = vec.size();

#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(ptr != nullptr)) {
    // Direct memory access for better performance
    for (R_xlen_t i = 0; i < len; ++i) {
      if (CPP4R_LIKELY(ptr[i] != NA_INTEGER)) {
        result += ptr[i];
      }
    }
  } else {
#else
  if (__builtin_expect(ptr != nullptr, 1)) {
    // Direct memory access for better performance
    for (R_xlen_t i = 0; i < len; ++i) {
      if (__builtin_expect(ptr[i] != NA_INTEGER, 1)) {
        result += ptr[i];
      }
    }
  } else {
#endif
    // Fallback to iterator approach
    for (auto it = vec.cbegin(); it != vec.cend(); ++it) {
      int val = *it;
#if CPP4R_HAS_CXX20
      if (CPP4R_LIKELY(val != NA_INTEGER)) {
        result += val;
      }
#else
      if (__builtin_expect(val != NA_INTEGER, 1)) {
        result += val;
      }
#endif
    }
  }
  return result;
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
