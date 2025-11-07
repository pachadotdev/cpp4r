#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <algorithm>         // for min
#include <array>             // for array
#include <cmath>             // for floor
#include <cstdint>           // for uint8_t
#include <cstring>           // for memcmp, memcpy, memset
#include <initializer_list>  // for initializer_list
#include <stdexcept>         // for runtime_error, out_of_range

#include "Rversion.h"
#include "cpp4r/R.hpp"                // for RAW, SEXP, SEXPREC, Rf_allocVector
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/protect.hpp"          // for safe
#include "cpp4r/r_vector.hpp"         // for r_vector, r_vector<>::proxy
#include "cpp4r/sexp.hpp"             // for sexp

// Specializations for raws

namespace cpp4r {

namespace traits {
template <>
struct get_underlying_type<uint8_t> {
  using type = Rbyte;
};
}  // namespace traits

template <>
inline SEXPTYPE r_vector<uint8_t>::get_sexptype() {
  return RAWSXP;
}

template <>
inline typename r_vector<uint8_t>::underlying_type r_vector<uint8_t>::get_elt(
    SEXP x, R_xlen_t i) {
  // NOPROTECT: likely too costly to unwind protect every elt
  return RAW_ELT(x, i);
}

template <>
inline typename r_vector<uint8_t>::underlying_type const* r_vector<uint8_t>::get_const_p(
    bool is_altrep, SEXP data) noexcept {
  return RAW_OR_NULL(data);
}

template <>
inline typename r_vector<uint8_t>::underlying_type* r_vector<uint8_t>::get_p(
    bool is_altrep, SEXP data) noexcept {
  return CPP4R_UNLIKELY(is_altrep) ? nullptr : RAW(data);
}

template <>
inline void r_vector<uint8_t>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                          typename r_vector::underlying_type* buf) {
  // NOPROTECT: likely too costly to unwind protect here
  RAW_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<uint8_t>::const_iterator::use_buf(bool is_altrep) noexcept {
  return CPP4R_UNLIKELY(is_altrep);
}

typedef r_vector<uint8_t> raws;

namespace writable {

template <>
inline void r_vector<uint8_t>::set_elt(SEXP x, R_xlen_t i,
                                       typename r_vector::underlying_type value) {
  // NOPROTECT: Likely too costly to unwind protect every set elt
#if R_VERSION >= R_Version(4, 2, 0)
  SET_RAW_ELT(x, i, value);
#else
  RAW(x)[i] = value;
#endif
}

// Add constructor for initializer_list for the writable r_vector specialization
template <>
inline r_vector<uint8_t>::r_vector(std::initializer_list<uint8_t> il)
    : cpp4r::r_vector<uint8_t>(safe[Rf_allocVector](RAWSXP, il.size())),
      capacity_(il.size()) {
  auto it = il.begin();
  Rbyte* ptr = RAW(this->data_);

  if (CPP4R_LIKELY(ptr != nullptr)) {
    // Direct memory access for better performance
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      ptr[i] = static_cast<Rbyte>(*it);
    }
  } else {
    // Fallback to element-wise assignment
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      set_elt(data_, i, static_cast<underlying_type>(*it));
    }
  }
}

typedef r_vector<uint8_t> raws;

}  // namespace writable

// Forward declarations for conversion functions
typedef r_vector<int> integers;
typedef r_vector<double> doubles;
typedef r_vector<r_bool> logicals;

// Optimized as_raws conversion function
inline raws as_raws(SEXP x) {
  SEXPTYPE x_type = detail::r_typeof(x);
  if (CPP4R_LIKELY(x_type == RAWSXP)) {
    return raws(x);
  }

  // Get length once and check for early exit
  R_xlen_t len = Rf_length(x);
  if (CPP4R_UNLIKELY(len == 0)) {
    return writable::raws(static_cast<R_xlen_t>(0));
  }

  if (CPP4R_UNLIKELY(x_type == INTSXP)) {
    integers xn(x);
    writable::raws ret(len);

    // Use direct memory access when possible for better performance
    const int* src_ptr = INTEGER_OR_NULL(x);
    Rbyte* dest_ptr = RAW(ret.data());

    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(src_ptr[i] == NA_INTEGER)) {
          throw std::runtime_error("Cannot convert NA integer to raw");
        }
        if (CPP4R_UNLIKELY(src_ptr[i] < 0 || src_ptr[i] > 255)) {
          throw std::runtime_error(
              "Integer value out of range for raw conversion (0-255)");
        }
        dest_ptr[i] = static_cast<Rbyte>(src_ptr[i]);
      }
    } else {
      // Fallback to iterator-based approach
      auto it = xn.begin();
      for (R_xlen_t i = 0; i < len; ++i, ++it) {
        int val = *it;
        if (CPP4R_UNLIKELY(val == NA_INTEGER)) {
          throw std::runtime_error("Cannot convert NA integer to raw");
        }
        if (CPP4R_UNLIKELY(val < 0 || val > 255)) {
          throw std::runtime_error(
              "Integer value out of range for raw conversion (0-255)");
        }
        ret[i] = static_cast<uint8_t>(val);
      }
    }
    return ret;
  } else if (CPP4R_UNLIKELY(x_type == REALSXP)) {
    doubles xn(x);
    writable::raws ret(len);

    // Use direct memory access when possible
    const double* src_ptr = REAL_OR_NULL(x);
    Rbyte* dest_ptr = RAW(ret.data());

    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(ISNA(src_ptr[i]))) {
          throw std::runtime_error("Cannot convert NA real to raw");
        }
        if (CPP4R_UNLIKELY(src_ptr[i] < 0.0 || src_ptr[i] > 255.0 ||
                           src_ptr[i] != floor(src_ptr[i]))) {
          throw std::runtime_error(
              "Real value out of range or not integer-like for raw conversion (0-255)");
        }
        dest_ptr[i] = static_cast<Rbyte>(src_ptr[i]);
      }
    } else {
      // Fallback to iterator-based approach
      auto it = xn.begin();
      for (R_xlen_t i = 0; i < len; ++i, ++it) {
        double val = *it;
        if (CPP4R_UNLIKELY(ISNA(val))) {
          throw std::runtime_error("Cannot convert NA real to raw");
        }
        if (CPP4R_UNLIKELY(val < 0.0 || val > 255.0 || val != floor(val))) {
          throw std::runtime_error(
              "Real value out of range or not integer-like for raw conversion (0-255)");
        }
        ret[i] = static_cast<uint8_t>(val);
      }
    }
    return ret;
  } else if (CPP4R_UNLIKELY(x_type == LGLSXP)) {
    logicals xn(x);
    writable::raws ret(len);

    // Use direct memory access when possible
    const int* src_ptr = LOGICAL_OR_NULL(x);
    Rbyte* dest_ptr = RAW(ret.data());

    if (CPP4R_LIKELY(src_ptr != nullptr && dest_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(src_ptr[i] == NA_LOGICAL)) {
          throw std::runtime_error("Cannot convert NA logical to raw");
        }
        dest_ptr[i] = src_ptr[i] ? static_cast<Rbyte>(1) : static_cast<Rbyte>(0);
      }
    } else {
      // Fallback to iterator-based approach
      auto it = xn.begin();
      for (R_xlen_t i = 0; i < len; ++i, ++it) {
        r_bool val = *it;
        if (CPP4R_UNLIKELY(val == NA_LOGICAL)) {
          throw std::runtime_error("Cannot convert NA logical to raw");
        }
        ret[i] =
            static_cast<bool>(val) ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0);
      }
    }
    return ret;
  }

  throw type_error(RAWSXP, x_type);
}

// Optimized comparison operators for r_vector<uint8_t>
template <>
inline bool operator==(const r_vector<uint8_t>& lhs, const r_vector<uint8_t>& rhs) {
  if (CPP4R_UNLIKELY(lhs.size() != rhs.size())) return false;

  // Fast path: if both vectors point to the same data, they're equal
  if (CPP4R_UNLIKELY(lhs.data() == rhs.data())) return true;

  // Use direct memory comparison when possible - very efficient for raw bytes
  const Rbyte* lhs_ptr = RAW_OR_NULL(lhs.data());
  const Rbyte* rhs_ptr = RAW_OR_NULL(rhs.data());

  if (CPP4R_LIKELY(lhs_ptr != nullptr && rhs_ptr != nullptr)) {
    R_xlen_t len = lhs.size();
    // Use memcmp for maximum performance on byte arrays
    return memcmp(lhs_ptr, rhs_ptr, len) == 0;
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
inline bool operator!=(const r_vector<uint8_t>& lhs, const r_vector<uint8_t>& rhs) {
  return !(lhs == rhs);
}

// Optimized utility functions for raw vectors
inline void raw_copy(const r_vector<uint8_t>& src, writable::raws& dst,
                     R_xlen_t src_start = 0, R_xlen_t dst_start = 0,
                     R_xlen_t length = -1) {
  R_xlen_t src_len = src.size();
  R_xlen_t dst_len = dst.size();

  if (length == -1) {
    length = std::min(src_len - src_start, dst_len - dst_start);
  }

  if (CPP4R_UNLIKELY(src_start + length > src_len || dst_start + length > dst_len)) {
    throw std::out_of_range("Copy operation would exceed vector bounds");
  }

  const Rbyte* src_ptr = RAW_OR_NULL(src.data());
  Rbyte* dst_ptr = RAW(dst.data());

  if (CPP4R_LIKELY(src_ptr != nullptr && dst_ptr != nullptr)) {
    // Use memcpy for maximum performance
    memcpy(dst_ptr + dst_start, src_ptr + src_start, length);
  } else {
    // Fallback to element-wise copy
    for (R_xlen_t i = 0; i < length; ++i) {
      dst[dst_start + i] = src[src_start + i];
    }
  }
}

inline void raw_fill(writable::raws& vec, uint8_t value, R_xlen_t start = 0,
                     R_xlen_t length = -1) {
  R_xlen_t vec_len = vec.size();

  if (length == -1) {
    length = vec_len - start;
  }

  if (CPP4R_UNLIKELY(start + length > vec_len)) {
    throw std::out_of_range("Fill operation would exceed vector bounds");
  }

  Rbyte* ptr = RAW(vec.data());
  if (CPP4R_LIKELY(ptr != nullptr)) {
    // Use memset for maximum performance
    memset(ptr + start, static_cast<int>(value), length);
  } else {
    // Fallback to element-wise assignment
    for (R_xlen_t i = start; i < start + length; ++i) {
      vec[i] = value;
    }
  }
}

inline writable::raws raw_concat(const r_vector<uint8_t>& lhs,
                                 const r_vector<uint8_t>& rhs) {
  R_xlen_t lhs_len = lhs.size();
  R_xlen_t rhs_len = rhs.size();
  R_xlen_t total_len = lhs_len + rhs_len;

  writable::raws result(total_len);

  const Rbyte* lhs_ptr = RAW_OR_NULL(lhs.data());
  const Rbyte* rhs_ptr = RAW_OR_NULL(rhs.data());
  Rbyte* result_ptr = RAW(result.data());

  if (CPP4R_LIKELY(lhs_ptr != nullptr && rhs_ptr != nullptr && result_ptr != nullptr)) {
    // Use memcpy for maximum performance
    memcpy(result_ptr, lhs_ptr, lhs_len);
    memcpy(result_ptr + lhs_len, rhs_ptr, rhs_len);
  } else {
    // Fallback to element-wise copy
    for (R_xlen_t i = 0; i < lhs_len; ++i) {
      result[i] = lhs[i];
    }
    for (R_xlen_t i = 0; i < rhs_len; ++i) {
      result[lhs_len + i] = rhs[i];
    }
  }

  return result;
}

}  // namespace cpp4r
