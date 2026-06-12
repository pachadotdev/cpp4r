#pragma once

#include <initializer_list>

#include "R_ext/Arith.h"
#include "cpp4r/R.hpp"
#include "cpp4r/as.hpp"
#include "cpp4r/cpp_version.hpp"
#include "cpp4r/protect.hpp"
#include "cpp4r/r_bool.hpp"
#include "cpp4r/r_vector.hpp"
#include "cpp4r/sexp.hpp"

namespace cpp4r {

template <>
inline SEXPTYPE r_vector<int>::get_sexptype() {
  return INTSXP;
}

template <>
inline typename r_vector<int>::underlying_type r_vector<int>::get_elt(SEXP x,
                                                                      R_xlen_t i) {
  return INTEGER_ELT(x, i);
}

template <>
inline typename r_vector<int>::underlying_type* r_vector<int>::get_p(bool is_altrep,
                                                                     SEXP data) {
  // Return nullptr for ALTREP so subscript falls through to INTEGER_ELT, which is
  // dispatched by the ALTREP class without materializing the entire vector.
  // cpp4r uses `const T*` as the const_iterator type for primitive vectors,
  // so we must always return a contiguous, materialized pointer here. ALTREP
  // is materialized by INTEGER(); the lazy fast path lives in `get_const_p`
  // and in the chunked GET_REGION code in `as_integers`.
  (void)is_altrep;
  return INTEGER(data);
}

template <>
inline typename r_vector<int>::underlying_type const* r_vector<int>::get_const_p(
    bool is_altrep, SEXP data) {
  (void)is_altrep;
  return INTEGER_OR_NULL(data);
}

template <>
inline void r_vector<int>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                      typename r_vector::underlying_type* buf) {
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
  SET_INTEGER_ELT(x, i, value);
}

typedef r_vector<int> integers;

}  // namespace writable

template <>
inline int na() {
  return NA_INTEGER;
}

typedef r_vector<double> doubles;
typedef r_vector<r_bool> logicals;

inline integers as_integers(SEXP x) {
  SEXPTYPE type = detail::r_typeof(x);
  if (type == INTSXP) return integers(x);

  if (type == REALSXP) {
    R_xlen_t len = Rf_xlength(x);
    const int no_na = REAL_NO_NA(x);
    const bool is_alt = ALTREP(x);

    writable::integers ret(len);
    int* CPP4R_RESTRICT dst = INTEGER(ret.data());
    // R's allocator guarantees alignment; hint to compiler for SIMD.
    dst = CPP4R_ASSUME_ALIGNED(dst, 4);
    CPP4R_ASSUME(len >= 0);
    CPP4R_ASSUME(dst != nullptr);

    if (!is_alt) {
      const double* CPP4R_RESTRICT src = REAL(x);
      // Validate all values are integer-like.
      if (no_na) {
        for (R_xlen_t i = 0; i < len; ++i) {
          if (!is_convertible_without_loss_to_integer(src[i])) {
            throw std::runtime_error("All elements must be integer-like");
          }
        }
        CPP4R_VECTORIZE
        for (R_xlen_t i = 0; i < len; ++i) {
          dst[i] = static_cast<int>(src[i]);
        }
      } else {
        for (R_xlen_t i = 0; i < len; ++i) {
          if (!ISNA(src[i]) && !is_convertible_without_loss_to_integer(src[i])) {
            throw std::runtime_error("All elements must be integer-like");
          }
        }
        for (R_xlen_t i = 0; i < len; ++i) {
          dst[i] = ISNA(src[i]) ? NA_INTEGER : static_cast<int>(src[i]);
        }
      }
      return ret;
    }

    // ALTREP source: stream in stack-sized chunks via REAL_GET_REGION so the
    // ALTREP class can serve runs without materializing the full vector.
    constexpr R_xlen_t kChunk = 512;
    double buf[kChunk];
    R_xlen_t i = 0;
    while (i < len) {
      const R_xlen_t n = (len - i < kChunk) ? (len - i) : kChunk;
      REAL_GET_REGION(x, i, n, buf);
      if (no_na) {
        for (R_xlen_t k = 0; k < n; ++k) {
          if (!is_convertible_without_loss_to_integer(buf[k])) {
            throw std::runtime_error("All elements must be integer-like");
          }
        }
        CPP4R_VECTORIZE
        for (R_xlen_t k = 0; k < n; ++k) {
          dst[i + k] = static_cast<int>(buf[k]);
        }
      } else {
        for (R_xlen_t k = 0; k < n; ++k) {
          if (ISNA(buf[k])) {
            dst[i + k] = NA_INTEGER;
          } else {
            if (!is_convertible_without_loss_to_integer(buf[k])) {
              throw std::runtime_error("All elements must be integer-like");
            }
            dst[i + k] = static_cast<int>(buf[k]);
          }
        }
      }
      i += n;
    }
    return ret;
  }

  if (type == LGLSXP) {
    R_xlen_t len = Rf_xlength(x);
    writable::integers ret(len);
    int* CPP4R_RESTRICT dst = INTEGER(ret.data());
    dst = CPP4R_ASSUME_ALIGNED(dst, 4);
    CPP4R_ASSUME(len >= 0);
    CPP4R_ASSUME(dst != nullptr);
    const int no_na = LOGICAL_NO_NA(x);

    if (!ALTREP(x)) {
      const int* CPP4R_RESTRICT src = LOGICAL(x);
      if (no_na) {
        CPP4R_VECTORIZE
        for (R_xlen_t i = 0; i < len; ++i) {
          dst[i] = src[i];
        }
      } else {
        for (R_xlen_t i = 0; i < len; ++i) {
          dst[i] = (src[i] == NA_LOGICAL) ? NA_INTEGER : src[i];
        }
      }
      return ret;
    }

    // ALTREP logical source: stream via LOGICAL_GET_REGION.
    constexpr R_xlen_t kChunk = 1024;
    int buf[kChunk];
    R_xlen_t i = 0;
    while (i < len) {
      const R_xlen_t n = (len - i < kChunk) ? (len - i) : kChunk;
      LOGICAL_GET_REGION(x, i, n, buf);
      if (no_na) {
        CPP4R_VECTORIZE
        for (R_xlen_t k = 0; k < n; ++k) {
          dst[i + k] = buf[k];
        }
      } else {
        for (R_xlen_t k = 0; k < n; ++k) {
          dst[i + k] = (buf[k] == NA_LOGICAL) ? NA_INTEGER : buf[k];
        }
      }
      i += n;
    }
    return ret;
  }

  throw type_error(INTSXP, type);
}

}  // namespace cpp4r
