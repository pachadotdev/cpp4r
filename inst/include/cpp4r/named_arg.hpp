#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

// for size_t [@MichaelChirico, r-lib/cpp11/pull/454]
// https://stackoverflow.com/questions/5079325/should-i-include-stddef-h-or-cstddef-for-size-t
#include <cstddef>
#include <cstring>     // for strcmp
#include <functional>  // for hash

#include <initializer_list>  // for initializer_list
#include <utility>           // for forward

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#include "cpp4r/R.hpp"     // for SEXP, SEXPREC, literals
#include "cpp4r/as.hpp"    // for as_sexp
#include "cpp4r/sexp.hpp"  // for sexp

namespace cpp4r {
class named_arg {
 public:
  explicit named_arg(const char* name) noexcept : name_(name), value_(R_NilValue) {}

  // Copy constructor and assignment for efficient copying
  named_arg(const named_arg& other) = default;
  named_arg& operator=(const named_arg& other) = default;

  // Move constructor and assignment for efficient moving
  named_arg(named_arg&& other) noexcept = default;
  named_arg& operator=(named_arg&& other) noexcept = default;

  // Constructor with immediate value assignment for efficiency
  template <typename T>
  explicit named_arg(const char* name, T&& value)
      : name_(name), value_(as_sexp(std::forward<T>(value))) {}

  // Specific overloads for common scalar types (non-explicit for convenience)
  named_arg(const char* name, double value) : name_(name), value_(as_sexp(value)) {}
  named_arg(const char* name, int value) : name_(name), value_(as_sexp(value)) {}
  named_arg(const char* name, bool value) : name_(name), value_(as_sexp(value)) {}
  named_arg(const char* name, const char* value) : name_(name), value_(as_sexp(value)) {}
  named_arg(const char* name, const std::string& value) : name_(name), value_(as_sexp(value)) {}

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload for zero-copy construction
  named_arg(const char* name, std::string_view value) : name_(name), value_(as_sexp(value)) {}
#endif

  named_arg& operator=(std::initializer_list<int> il) {
    value_ = as_sexp(il);
    return *this;
  }

  template <typename T>
  named_arg& operator=(T&& rhs) & {
    value_ = as_sexp(std::forward<T>(rhs));
    return *this;
  }

  // Rvalue overload for use in temporary contexts (like push_back)
  template <typename T>
  named_arg&& operator=(T&& rhs) && {
    value_ = as_sexp(std::forward<T>(rhs));
    return std::move(*this);
  }

  template <typename T>
  named_arg& operator=(std::initializer_list<T> rhs) {
    value_ = as_sexp(rhs);
    return *this;
  }

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD const char* name() const noexcept { return name_; }
  CPP4R_NODISCARD SEXP value() const noexcept { return value_; }
#else
  const char* name() const noexcept { return name_; }
  SEXP value() const noexcept { return value_; }
#endif

  // Comparison operators for efficient name-based comparison
  bool operator==(const named_arg& other) const noexcept {
#if CPP4R_HAS_CXX20
    return CPP4R_LIKELY(std::strcmp(name_, other.name_) == 0);
#else
    return std::strcmp(name_, other.name_) == 0;
#endif
  }

  bool operator!=(const named_arg& other) const noexcept { return !(*this == other); }

  bool operator==(const char* other_name) const noexcept {
#if CPP4R_HAS_CXX20
    return CPP4R_LIKELY(std::strcmp(name_, other_name) == 0);
#else
    return std::strcmp(name_, other_name) == 0;
#endif
  }

  bool operator!=(const char* other_name) const noexcept {
#if CPP4R_HAS_CXX20
    return CPP4R_UNLIKELY(std::strcmp(name_, other_name) != 0);
#else
    return std::strcmp(name_, other_name) != 0;
#endif
  }

 private:
  const char* name_;
  sexp value_;
};

namespace literals {

#if CPP4R_HAS_CXX17
CPP4R_NODISCARD inline named_arg operator""_nm(const char* name, std::size_t) { 
  return named_arg(name); 
}
#else
inline named_arg operator""_nm(const char* name, std::size_t) { 
  return named_arg(name); 
}
#endif

}  // namespace literals

// Free function comparison operators for symmetry
inline bool operator==(const char* lhs, const named_arg& rhs) noexcept {
  return rhs == lhs;
}

inline bool operator!=(const char* lhs, const named_arg& rhs) noexcept {
  return rhs != lhs;
}

using namespace literals;

}  // namespace cpp4r

// Hash specialization for use in std::unordered_map and similar containers
namespace std {
template <>
struct hash<cpp4r::named_arg> {
#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD std::size_t operator()(const cpp4r::named_arg& arg) const noexcept {
    return std::hash<const char*>{}(arg.name());
  }
#else
  std::size_t operator()(const cpp4r::named_arg& arg) const noexcept {
    return std::hash<const char*>{}(arg.name());
  }
#endif
};
}  // namespace std
