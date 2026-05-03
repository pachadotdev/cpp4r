#pragma once

#include <initializer_list>

#include "cpp4r/R.hpp"
#include "cpp4r/protect.hpp"
#include "cpp4r/r_bool.hpp"
#include "cpp4r/r_vector.hpp"
#include "cpp4r/sexp.hpp"

namespace cpp4r {

template <>
inline SEXPTYPE r_vector<r_bool>::get_sexptype() {
  return LGLSXP;
}

template <>
inline typename r_vector<r_bool>::underlying_type r_vector<r_bool>::get_elt(SEXP x,
                                                                            R_xlen_t i) {
  return LOGICAL_ELT(x, i);
}

template <>
inline typename r_vector<r_bool>::underlying_type* r_vector<r_bool>::get_p(bool is_altrep,
                                                                           SEXP data) {
  // Return nullptr for ALTREP so subscript falls through to LOGICAL_ELT, which is
  // dispatched by the ALTREP class without materializing the entire vector.
  // cpp4r uses `const T*` as the const_iterator type for primitive vectors,
  // so we must always return a contiguous, materialized pointer here.
  (void)is_altrep;
  return LOGICAL(data);
}

template <>
inline typename r_vector<r_bool>::underlying_type const* r_vector<r_bool>::get_const_p(
    bool is_altrep, SEXP data) {
  return LOGICAL_OR_NULL(data);
}

template <>
inline void r_vector<r_bool>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                         typename r_vector::underlying_type* buf) {
  LOGICAL_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<r_bool>::const_iterator::use_buf(bool is_altrep) {
  return is_altrep;
}

typedef r_vector<r_bool> logicals;

namespace writable {

template <>
inline void r_vector<r_bool>::set_elt(SEXP x, R_xlen_t i,
                                      typename r_vector::underlying_type value) {
  SET_LOGICAL_ELT(x, i, value);
}

inline bool operator==(const r_vector<r_bool>::proxy& lhs, r_bool rhs) {
  return static_cast<r_bool>(lhs).operator==(rhs);
}

typedef r_vector<r_bool> logicals;

}  // namespace writable

}  // namespace cpp4r
