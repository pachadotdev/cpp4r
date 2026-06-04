#pragma once
#include <cpp4r.hpp>
using namespace cpp4r;

[[cpp4r::register]]
bool weak_ref_make_alive_(SEXP key, SEXP val) {
  cpp4r::weak_ref wr(key, val);
  return wr.alive();
}

[[cpp4r::register]]
SEXP weak_ref_value_(SEXP key, SEXP val) {
  cpp4r::weak_ref wr(key, val);
  return wr.value();
}

[[cpp4r::register]]
int weak_ref_typeof_(SEXP key, SEXP val) {
  cpp4r::weak_ref wr(key, val);
  return TYPEOF(SEXP(wr));
}

[[cpp4r::register]]
bool weak_ref_nil_not_alive_() {
  cpp4r::weak_ref wr(R_NilValue, R_NilValue);
  return wr.alive();
}

[[cpp4r::register]]
void weak_ref_rejects_vec_() {
  SEXP vec = PROTECT(Rf_allocVector(INTSXP, 1));
  cpp4r::weak_ref wr(vec, R_NilValue);
  UNPROTECT(1);
}
