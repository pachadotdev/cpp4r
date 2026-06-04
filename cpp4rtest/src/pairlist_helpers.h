#pragma once
#include <cpp4r.hpp>
using namespace cpp4r;

[[cpp4r::register]]
int pairlist_size_(SEXP pl) {
  cpp4r::pairlist p(pl);
  return static_cast<int>(p.size());
}

[[cpp4r::register]]
list pairlist_to_list_(SEXP pl) {
  cpp4r::pairlist p(pl);
  return p.to_list();
}

[[cpp4r::register]]
void pairlist_rejects_vec_() {
  SEXP vec = PROTECT(Rf_allocVector(INTSXP, 3));
  cpp4r::pairlist p(vec);
  UNPROTECT(1);
}
