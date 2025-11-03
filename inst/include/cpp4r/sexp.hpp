#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <stddef.h>   // for size_t
#include <stdexcept>  // for invalid_argument

#include <string>  // for string, basic_string

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

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
        preserve_token_(CPP4R_UNLIKELY(data == R_NilValue)
                            ? R_NilValue
                            : detail::store::insert(data)) {}

  // We maintain our own new `preserve_token_`
  sexp(const sexp& rhs) : data_(rhs.data_) {
    // Only insert if data is not R_NilValue to avoid unnecessary work
    preserve_token_ = CPP4R_UNLIKELY(data_ == R_NilValue)
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
    if (CPP4R_LIKELY(this != &rhs)) {  // Self-assignment check - expect it's not self-assignment
      detail::store::release(preserve_token_);

      data_ = rhs.data_;
      preserve_token_ = CPP4R_UNLIKELY(data_ == R_NilValue)
                            ? R_NilValue
                            : detail::store::insert(data_);
    }

    return *this;
  }

  sexp& operator=(sexp&& rhs) noexcept {
    if (CPP4R_LIKELY(this != &rhs)) {  // Self-assignment check - expect it's not self-assignment
      detail::store::release(preserve_token_);

      data_ = rhs.data_;
      preserve_token_ = rhs.preserve_token_;

      rhs.data_ = R_NilValue;
      rhs.preserve_token_ = R_NilValue;
    }

    return *this;
  }

  ~sexp() {
    if (CPP4R_LIKELY(preserve_token_ != R_NilValue)) {
      detail::store::release(preserve_token_);
    }
  }

  attribute_proxy<sexp> attr(const char* name) const {
    return attribute_proxy<sexp>(*this, name);
  }

  attribute_proxy<sexp> attr(const std::string& name) const {
    return attribute_proxy<sexp>(*this, name.c_str());
  }

#if CPP4R_HAS_CXX17
  // C++17: Constructor for std::string_view (avoids string copies)
  attribute_proxy<sexp> attr(std::string_view name) const {
    return attribute_proxy<sexp>(*this, name.data());
  }
#endif

  attribute_proxy<sexp> attr(SEXP name) const {
    return attribute_proxy<sexp>(*this, name);
  }

  attribute_proxy<sexp> names() const {
    return attribute_proxy<sexp>(*this, R_NamesSymbol);
  }

  CPP4R_NODISCARD operator SEXP() const { return data_; }
  CPP4R_NODISCARD SEXP data() const { return data_; }

  // Implicit conversion operators for common types
  CPP4R_NODISCARD operator double() const {
    if (CPP4R_LIKELY(TYPEOF(data_) == REALSXP && Rf_length(data_) == 1)) {
      return REAL_ELT(data_, 0);
    }
    if (CPP4R_UNLIKELY(TYPEOF(data_) == INTSXP && Rf_length(data_) == 1)) {
      return static_cast<double>(INTEGER_ELT(data_, 0));
    }
    throw std::invalid_argument("Cannot convert SEXP to double");
  }

  CPP4R_NODISCARD operator int() const {
    if (CPP4R_LIKELY(TYPEOF(data_) == INTSXP && Rf_length(data_) == 1)) {
      return INTEGER_ELT(data_, 0);
    }
    if (CPP4R_UNLIKELY(TYPEOF(data_) == REALSXP && Rf_length(data_) == 1)) {
      double val = REAL_ELT(data_, 0);
      return static_cast<int>(val);
    }
    throw std::invalid_argument("Cannot convert SEXP to int");
  }

  CPP4R_NODISCARD operator bool() const {
    if (CPP4R_LIKELY(TYPEOF(data_) == LGLSXP && Rf_length(data_) == 1)) {
      return LOGICAL_ELT(data_, 0) != 0;
    }
    throw std::invalid_argument("Cannot convert SEXP to bool");
  }

  // Optimized comparison operators
  CPP4R_NODISCARD bool operator==(const sexp& other) const noexcept { return data_ == other.data_; }

  CPP4R_NODISCARD bool operator!=(const sexp& other) const noexcept { return data_ != other.data_; }

  CPP4R_NODISCARD bool operator==(SEXP other) const noexcept { return data_ == other; }

  CPP4R_NODISCARD bool operator!=(SEXP other) const noexcept { return data_ != other; }
};

// Free function comparison operators for symmetry
CPP4R_NODISCARD inline bool operator==(SEXP lhs, const sexp& rhs) noexcept { return lhs == rhs.data(); }

CPP4R_NODISCARD inline bool operator!=(SEXP lhs, const sexp& rhs) noexcept { return lhs != rhs.data(); }

// Utility functions for common operations
CPP4R_NODISCARD inline bool is_null(const sexp& x) noexcept { return x.data() == R_NilValue; }

CPP4R_NODISCARD inline SEXPTYPE sexp_type(const sexp& x) noexcept { return detail::r_typeof(x.data()); }

CPP4R_NODISCARD inline R_xlen_t sexp_length(const sexp& x) noexcept { return Rf_length(x.data()); }

}  // namespace cpp4r
