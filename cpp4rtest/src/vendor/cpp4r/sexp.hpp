#pragma once

#include <stddef.h>     // for size_t
#include <string>       // for string, basic_string
#include <type_traits>  // for enable_if, is_same, decay

#include "cpp4r/R.hpp"                // for R’s C interface (e.g., for SEXP)
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/cpp_version.hpp"      // for CPP4R_HAS_CXX17
#include "cpp4r/protect.hpp"          // for store

namespace cpp4r {

/// Converting to SEXP
class sexp {
 private:
  SEXP data_ = R_NilValue;
  SEXP preserve_token_ = R_NilValue;

 public:
  sexp() noexcept = default;

  sexp(SEXP data) : data_(data), preserve_token_(detail::store::insert(data_)) {}

  // Templated constructor for types with operator SEXP()
  // This resolves ambiguity in C++14 between sexp(SEXP) and copy/move constructors
  template <typename T, typename = enable_if_t<!std::is_same<decay_t<T>, sexp>::value &&
                                               !std::is_same<decay_t<T>, SEXP>::value &&
                                               std::is_convertible<T, SEXP>::value>>
  sexp(T&& value) {
#if CPP4R_HAS_CXX17
    data_ = static_cast<SEXP>(std::forward<T>(value));
#else
    data_ = static_cast<SEXP>(value);
#endif
    preserve_token_ = detail::store::insert(data_);
  }

  // We maintain our own new `preserve_token_`
  sexp(const sexp& rhs) {
    data_ = rhs.data_;
    preserve_token_ = detail::store::insert(data_);
  }

  // We take ownership over the `rhs.preserve_token_`.
  // Importantly we clear it in the `rhs` so it can't release the object upon destruction.
  sexp(sexp&& rhs) noexcept {
    data_ = rhs.data_;
    preserve_token_ = rhs.preserve_token_;

    rhs.data_ = R_NilValue;
    rhs.preserve_token_ = R_NilValue;
  }

  sexp& operator=(const sexp& rhs) {
    detail::store::release(preserve_token_);

    data_ = rhs.data_;
    preserve_token_ = detail::store::insert(data_);

    return *this;
  }

  ~sexp() { detail::store::release(preserve_token_); }

  CPP4R_NODISCARD attribute_proxy<sexp> attr(const char* name) const {
    return attribute_proxy<sexp>(*this, name);
  }

  CPP4R_NODISCARD attribute_proxy<sexp> attr(const std::string& name) const {
    return attribute_proxy<sexp>(*this, name.c_str());
  }

#if CPP4R_HAS_CXX17
  // C++17+: accept string_view directly, avoiding a temporary std::string
  CPP4R_NODISCARD attribute_proxy<sexp> attr(std::string_view name) const {
    return attribute_proxy<sexp>(*this, name);
  }
#endif

  CPP4R_NODISCARD attribute_proxy<sexp> attr(SEXP name) const {
    return attribute_proxy<sexp>(*this, name);
  }

  CPP4R_NODISCARD attribute_proxy<sexp> names() const {
    return attribute_proxy<sexp>(*this, R_NamesSymbol);
  }

  CPP4R_ALWAYS_INLINE operator SEXP() const noexcept { return data_; }
  CPP4R_ALWAYS_INLINE SEXP data() const noexcept { return data_; }

  /// DEPRECATED: Do not use this, it will be removed soon.
  operator double() const { return REAL_ELT(data_, 0); }
  /// DEPRECATED: Do not use this, it will be removed soon.
  operator size_t() const { return REAL_ELT(data_, 0); }
  /// DEPRECATED: Do not use this, it will be removed soon.
  operator bool() const { return LOGICAL_ELT(data_, 0); }
};

}  // namespace cpp4r
