#pragma once
#include <cpp4r.hpp>
using namespace cpp4r;

/* roxygen
@title Weak Reference Helpers
@description Test suite
@param key key for weak reference
@param val value for weak reference
@export
*/
[[cpp4r::register]] bool weak_ref_make_alive_(SEXP key, SEXP val) {
  cpp4r::weak_ref wr(key, val);
  return wr.alive();
}

/* roxygen
@title Weak Reference Value
@description Test suite
@param key key for weak reference
@param val value for weak reference
@export
*/
[[cpp4r::register]] SEXP weak_ref_value_(SEXP key, SEXP val) {
  cpp4r::weak_ref wr(key, val);
  return wr.value();
}

/* roxygen
@title Weak Reference Type
@description Test suite
@param key key for weak reference
@param val value for weak reference
@export
*/
[[cpp4r::register]] int weak_ref_typeof_(SEXP key, SEXP val) {
  cpp4r::weak_ref wr(key, val);
  return TYPEOF(SEXP(wr));
}

/* roxygen
@title Weak Reference Nil Not Alive
@description Test suite
@export
*/
[[cpp4r::register]] bool weak_ref_nil_not_alive_() {
  cpp4r::weak_ref wr(R_NilValue, R_NilValue);
  return wr.alive();
}

/* roxygen
@title Weak Reference Rejects Vectors
@description Test suite
@export
*/
[[cpp4r::register]] void weak_ref_rejects_vec_() {
  SEXP vec = PROTECT(Rf_allocVector(INTSXP, 1));
  cpp4r::weak_ref wr(vec, R_NilValue);
  UNPROTECT(1);
}
