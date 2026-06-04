#pragma once

#include <initializer_list>  // for initializer_list
#include <string>            // for string, basic_string

// C++17+: std::string_view for zero-cost attribute name passing
#if CPP4R_HAS_CXX17
#include <string_view>
#endif

#include "cpp4r/R.hpp"        // for R’s C interface (e.g., for SEXP)
#include "cpp4r/as.hpp"       // for as_sexp
#include "cpp4r/protect.hpp"  // for protect, safe, protect::function

namespace cpp4r {

class sexp;

template <typename T>
class attribute_proxy {
 private:
  const T& parent_;
  SEXP symbol_;

 public:
  attribute_proxy(const T& parent, const char* index)
      : parent_(parent), symbol_(safe[Rf_install](index)) {}

  attribute_proxy(const T& parent, const std::string& index)
      : parent_(parent), symbol_(safe[Rf_install](index.c_str())) {}

#if CPP4R_HAS_CXX17
  // C++17+: accept string_view directly, avoiding a temporary std::string at call sites
  attribute_proxy(const T& parent, std::string_view index)
      : parent_(parent), symbol_(safe[Rf_install](std::string(index).c_str())) {}
#endif

  attribute_proxy(const T& parent, SEXP index) : parent_(parent), symbol_(index) {}

  template <typename C>
  attribute_proxy& operator=(C rhs) {
    SEXP value = PROTECT(as_sexp(rhs));
    Rf_setAttrib(parent_.data(), symbol_, value);
    UNPROTECT(1);
    return *this;
  }

  template <typename C>
  attribute_proxy& operator=(std::initializer_list<C> rhs) {
    SEXP value = PROTECT(as_sexp(rhs));
    Rf_setAttrib(parent_.data(), symbol_, value);
    UNPROTECT(1);
    return *this;
  }

  operator SEXP() const { return safe[Rf_getAttrib](parent_.data(), symbol_); }
};

}  // namespace cpp4r
