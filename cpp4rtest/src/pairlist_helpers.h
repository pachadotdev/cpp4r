#pragma once
#include <cpp4r.hpp>
using namespace cpp4r;

/* roxygen
@title Get Size of Pairlist (int output)
@description Test suite
@param pl pairlist to query
@export
*/
[[cpp4r::register]] int pairlist_size_(SEXP pl) {
  cpp4r::pairlist p(pl);
  return static_cast<int>(p.size());
}

/* roxygen
@title Convert Pairlist to List (list output)
@description Test suite
@param pl pairlist to convert
@export
*/
[[cpp4r::register]] list pairlist_to_list_(SEXP pl) {
  cpp4r::pairlist p(pl);
  return p.to_list();
}

/* roxygen
@title Reject Vector Input for Pairlist
@description Test suite
@export
*/
[[cpp4r::register]] void pairlist_rejects_vec_() {
  SEXP vec = PROTECT(Rf_allocVector(INTSXP, 3));
  cpp4r::pairlist p(vec);
  UNPROTECT(1);
}
