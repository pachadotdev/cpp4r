#pragma once

#include <initializer_list>  // for initializer_list
#include <string>            // for string, basic_string
#include <utility>           // for std::forward

#include "cpp4r/R.hpp"        // for SEXP, SEXPREC, Rf_install, PROTECT, Rf_...
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
  inline attribute_proxy(const T& parent, const char* index)
      : parent_(parent), symbol_(safe[Rf_install](index)) {}

  inline attribute_proxy(const T& parent, const std::string& index)
      : parent_(parent), symbol_(safe[Rf_install](index.c_str())) {}

  inline attribute_proxy(const T& parent, SEXP index) noexcept
      : parent_(parent), symbol_(index) {}

  template <typename C>
  inline attribute_proxy& operator=(C&& rhs) {
    // Universal reference for optimal forwarding
    unwind_protect([&] {
      SEXP value = as_sexp(std::forward<C>(rhs));
      Rf_setAttrib(parent_.data(), symbol_, value);
    });
    return *this;
  }

  template <typename C>
  inline attribute_proxy& operator=(std::initializer_list<C> rhs) {
    // Use unwind_protect for better exception safety and potentially faster execution
    unwind_protect([&] {
      SEXP value = as_sexp(rhs);
      Rf_setAttrib(parent_.data(), symbol_, value);
    });
    return *this;
  }

  inline operator SEXP() const noexcept {
    return safe[Rf_getAttrib](parent_.data(), symbol_);
  }
};

}  // namespace cpp4r
