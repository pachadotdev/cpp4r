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
inline SEXPTYPE r_vector<double>::get_sexptype() {
  return REALSXP;
}

template <>
inline typename r_vector<double>::underlying_type r_vector<double>::get_elt(SEXP x,
                                                                            R_xlen_t i) {
  return REAL_ELT(x, i);
}

template <>
inline typename r_vector<double>::underlying_type* r_vector<double>::get_p(bool is_altrep,
                                                                           SEXP data) {
  // cpp4r uses `const T*` as the const_iterator type for primitive vectors,
  // so we must always return a contiguous, materialized pointer here. ALTREP
  // is materialized by REAL(); the lazy fast path lives in `get_const_p` and
  // in the chunked GET_REGION code in `as_doubles`/`as_integers`.
  (void)is_altrep;
  return REAL(data);
}

template <>
inline typename r_vector<double>::underlying_type const* r_vector<double>::get_const_p(
    bool is_altrep, SEXP data) {
  (void)is_altrep;
  return REAL_OR_NULL(data);
}

template <>
inline void r_vector<double>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                         typename r_vector::underlying_type* buf) {
  REAL_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<double>::generic_const_iterator::use_buf(bool is_altrep) {
  return is_altrep;
}

typedef r_vector<double> doubles;

namespace writable {

template <>
inline void r_vector<double>::set_elt(SEXP x, R_xlen_t i,
                                      typename r_vector::underlying_type value) {
  SET_REAL_ELT(x, i, value);
}

typedef r_vector<double> doubles;

}  // namespace writable

template <>
inline const double* r_vector<double>::data_ptr() const noexcept {
  return data_p_;
}

namespace writable {
template <>
inline double* r_vector<double>::data_ptr_writable() noexcept {
  return data_p_;
}

template <>
inline const double* r_vector<double>::data_ptr() const noexcept {
  return data_p_;
}
}  // namespace writable

typedef r_vector<int> integers;
typedef r_vector<r_bool> logicals;

inline doubles as_doubles(SEXP x) {
  SEXPTYPE type = detail::r_typeof(x);
  if (type == REALSXP) return doubles(x);

  if (type == INTSXP || type == LGLSXP) {
    R_xlen_t len = Rf_xlength(x);
    writable::doubles ret(len);
    double* CPP4R_RESTRICT dst = REAL(ret.data());
    // R's allocator guarantees alignment; communicate this to the compiler
    // so it can emit aligned SIMD loads/stores on the destination.
    dst = CPP4R_ASSUME_ALIGNED(dst, 8);
    CPP4R_ASSUME(len >= 0);
    CPP4R_ASSUME(dst != nullptr);

    const bool is_alt = ALTREP(x);
    const int* CPP4R_RESTRICT src = nullptr;
    if (!is_alt) {
      src = (type == INTSXP) ? INTEGER(x) : LOGICAL(x);
      src = CPP4R_ASSUME_ALIGNED(src, 4);
    }

    // Fast path: when R guarantees no NAs we can do a branchless cast that
    // auto-vectorizes (the per-element NA test otherwise inhibits SIMD).
    const int no_na = (type == INTSXP) ? INTEGER_NO_NA(x) : LOGICAL_NO_NA(x);
    const int na_val = (type == INTSXP) ? NA_INTEGER : NA_LOGICAL;

    if (!is_alt) {
      if (no_na) {
        CPP4R_VECTORIZE
        for (R_xlen_t i = 0; i < len; ++i) {
          dst[i] = static_cast<double>(src[i]);
        }
      } else {
        for (R_xlen_t i = 0; i < len; ++i) {
          dst[i] = (src[i] == na_val) ? NA_REAL : static_cast<double>(src[i]);
        }
      }
      return ret;
    }

    // ALTREP source: read in stack-sized chunks via *_GET_REGION so the ALTREP
    // class can serve runs analytically (e.g. compact integer ranges) without
    // materializing the whole vector.
    constexpr R_xlen_t kChunk = 1024;
    int buf[kChunk];
    R_xlen_t i = 0;
    while (i < len) {
      const R_xlen_t n = (len - i < kChunk) ? (len - i) : kChunk;
      if (type == INTSXP) {
        INTEGER_GET_REGION(x, i, n, buf);
      } else {
        LOGICAL_GET_REGION(x, i, n, buf);
      }
      if (no_na) {
        CPP4R_VECTORIZE
        for (R_xlen_t k = 0; k < n; ++k) {
          dst[i + k] = static_cast<double>(buf[k]);
        }
      } else {
        for (R_xlen_t k = 0; k < n; ++k) {
          dst[i + k] = (buf[k] == na_val) ? NA_REAL : static_cast<double>(buf[k]);
        }
      }
      i += n;
    }
    return ret;
  }

  throw type_error(REALSXP, type);
}

template <>
inline double na() {
  return NA_REAL;
}

}  // namespace cpp4r
