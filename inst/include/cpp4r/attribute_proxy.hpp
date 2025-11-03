#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <initializer_list>  // for initializer_list
#include <string>            // for string, basic_string
#include <utility>           // for std::forward

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

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
  // C++11/14/17/20: Constructor for C-string
  inline attribute_proxy(const T& parent, const char* index)
      : parent_(parent), symbol_(safe[Rf_install](index)) {}

  // C++11/14/17/20: Constructor for std::string
  inline attribute_proxy(const T& parent, const std::string& index)
      : parent_(parent), symbol_(safe[Rf_install](index.c_str())) {}

#if CPP4R_HAS_CXX17
  // C++17: Constructor for std::string_view (avoids string copies)
  inline attribute_proxy(const T& parent, std::string_view index)
      : parent_(parent), symbol_(safe[Rf_install](index.data())) {}
#endif

  // C++11/14/17/20: Constructor for SEXP
  inline attribute_proxy(const T& parent, SEXP index) noexcept
      : parent_(parent), symbol_(index) {}

  template <typename C>
#if CPP4R_HAS_CXX20
  // C++20: Use concepts for better type constraints (future enhancement)
  inline attribute_proxy& operator=(C&& rhs) CPP4R_UNLIKELY {
#else
  inline attribute_proxy& operator=(C&& rhs) {
#endif
    // Universal reference for optimal forwarding
#if CPP4R_HAS_CXX17
    // C++17: More efficient with guaranteed copy elision
    unwind_protect([&] {
      SEXP value = as_sexp(std::forward<C>(rhs));
      Rf_setAttrib(parent_.data(), symbol_, value);
    });
#else
    // C++11/14: Standard approach
    unwind_protect([&] {
      SEXP value = as_sexp(std::forward<C>(rhs));
      Rf_setAttrib(parent_.data(), symbol_, value);
    });
#endif
    return *this;
  }

  template <typename C>
  inline attribute_proxy& operator=(std::initializer_list<C> rhs) {
#if CPP4R_HAS_CXX17
    // C++17: Guaranteed copy elision and better optimization
    unwind_protect([&] {
      SEXP value = as_sexp(rhs);
      Rf_setAttrib(parent_.data(), symbol_, value);
    });
#else
    // C++11/14: Standard approach
    unwind_protect([&] {
      SEXP value = as_sexp(rhs);
      Rf_setAttrib(parent_.data(), symbol_, value);
    });
#endif
    return *this;
  }

#if CPP4R_HAS_CXX17
  // C++17: Mark as [[nodiscard]] to warn about unused results
  CPP4R_NODISCARD inline operator SEXP() const noexcept {
    return safe[Rf_getAttrib](parent_.data(), symbol_);
  }
#else
  // C++11/14: No nodiscard attribute
  inline operator SEXP() const noexcept {
    return safe[Rf_getAttrib](parent_.data(), symbol_);
  }
#endif
};

}  // namespace cpp4r
