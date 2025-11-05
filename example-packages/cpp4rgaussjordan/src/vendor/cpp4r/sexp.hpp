#pragma once

#include <stddef.h>   // for size_t
#include <stdexcept>  // for invalid_argument

#include <string>  // for string, basic_string

#include "cpp4r/R.hpp"                // for SEXP, SEXPREC, REAL_ELT, R_NilV...
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/protect.hpp"          // for store

namespace cpp4r {

/// Converting to SEXP
class sexp {
 private:
  SEXP data_ = R_NilValue;
  SEXP preserve_token_ = R_NilValue;

 public:
  sexp() = default;

  sexp(SEXP data)
      : data_(data),
        preserve_token_(__builtin_expect(data == R_NilValue, 0)
                            ? R_NilValue
                            : detail::store::insert(data)) {}

  // We maintain our own new `preserve_token_`
  sexp(const sexp& rhs) : data_(rhs.data_) {
    // Only insert if data is not R_NilValue to avoid unnecessary work
    preserve_token_ = __builtin_expect(data_ == R_NilValue, 0)
                          ? R_NilValue
                          : detail::store::insert(data_);
  }

  // We take ownership over the `rhs.preserve_token_`.
  // Importantly we clear it in the `rhs` so it can't release the object upon destruction.
  sexp(sexp&& rhs) {
    data_ = rhs.data_;
    preserve_token_ = rhs.preserve_token_;

    rhs.data_ = R_NilValue;
    rhs.preserve_token_ = R_NilValue;
  }

  sexp& operator=(const sexp& rhs) {
    if (__builtin_expect(this != &rhs,
                         1)) {  // Self-assignment check - expect it's not self-assignment
      detail::store::release(preserve_token_);

      data_ = rhs.data_;
      preserve_token_ = __builtin_expect(data_ == R_NilValue, 0)
                            ? R_NilValue
                            : detail::store::insert(data_);
    }

    return *this;
  }

  sexp& operator=(sexp&& rhs) noexcept {
    if (__builtin_expect(this != &rhs,
                         1)) {  // Self-assignment check - expect it's not self-assignment
      detail::store::release(preserve_token_);

      data_ = rhs.data_;
      preserve_token_ = rhs.preserve_token_;

      rhs.data_ = R_NilValue;
      rhs.preserve_token_ = R_NilValue;
    }

    return *this;
  }

  ~sexp() {
    if (__builtin_expect(preserve_token_ != R_NilValue, 1)) {
      detail::store::release(preserve_token_);
    }
  }

  attribute_proxy<sexp> attr(const char* name) const {
    return attribute_proxy<sexp>(*this, name);
  }

  attribute_proxy<sexp> attr(const std::string& name) const {
    return attribute_proxy<sexp>(*this, name.c_str());
  }

  attribute_proxy<sexp> attr(SEXP name) const {
    return attribute_proxy<sexp>(*this, name);
  }

  attribute_proxy<sexp> names() const {
    return attribute_proxy<sexp>(*this, R_NamesSymbol);
  }

  operator SEXP() const { return data_; }
  SEXP data() const { return data_; }

  // Implicit conversion operators for common types
  operator double() const {
    if (__builtin_expect(TYPEOF(data_) == REALSXP && Rf_length(data_) == 1, 1)) {
      return REAL_ELT(data_, 0);
    }
    if (__builtin_expect(TYPEOF(data_) == INTSXP && Rf_length(data_) == 1, 0)) {
      return static_cast<double>(INTEGER_ELT(data_, 0));
    }
    throw std::invalid_argument("Cannot convert SEXP to double");
  }

  operator int() const {
    if (__builtin_expect(TYPEOF(data_) == INTSXP && Rf_length(data_) == 1, 1)) {
      return INTEGER_ELT(data_, 0);
    }
    if (__builtin_expect(TYPEOF(data_) == REALSXP && Rf_length(data_) == 1, 0)) {
      double val = REAL_ELT(data_, 0);
      return static_cast<int>(val);
    }
    throw std::invalid_argument("Cannot convert SEXP to int");
  }

  operator bool() const {
    if (__builtin_expect(TYPEOF(data_) == LGLSXP && Rf_length(data_) == 1, 1)) {
      return LOGICAL_ELT(data_, 0) != 0;
    }
    throw std::invalid_argument("Cannot convert SEXP to bool");
  }

  // Optimized comparison operators
  bool operator==(const sexp& other) const noexcept { return data_ == other.data_; }

  bool operator!=(const sexp& other) const noexcept { return data_ != other.data_; }

  bool operator==(SEXP other) const noexcept { return data_ == other; }

  bool operator!=(SEXP other) const noexcept { return data_ != other; }
};

// Free function comparison operators for symmetry
inline bool operator==(SEXP lhs, const sexp& rhs) noexcept { return lhs == rhs.data(); }

inline bool operator!=(SEXP lhs, const sexp& rhs) noexcept { return lhs != rhs.data(); }

// Utility functions for common operations
inline bool is_null(const sexp& x) noexcept { return x.data() == R_NilValue; }

inline SEXPTYPE sexp_type(const sexp& x) noexcept { return detail::r_typeof(x.data()); }

inline R_xlen_t sexp_length(const sexp& x) noexcept { return Rf_length(x.data()); }

}  // namespace cpp4r
