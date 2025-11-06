#pragma once

// for size_t [@MichaelChirico, r-lib/cpp11/pull/454]
// https://stackoverflow.com/questions/5079325/should-i-include-stddef-h-or-cstddef-for-size-t
#include <cstddef>
#include <cstring>     // for strcmp
#include <functional>  // for hash

#include <initializer_list>  // for initializer_list
#include <utility>           // for forward

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

  named_arg& operator=(std::initializer_list<int> il) {
    value_ = as_sexp(il);
    return *this;
  }

  template <typename T>
  named_arg& operator=(T&& rhs) {
    value_ = as_sexp(std::forward<T>(rhs));
    return *this;
  }

  template <typename T>
  named_arg& operator=(std::initializer_list<T> rhs) {
    value_ = as_sexp(rhs);
    return *this;
  }

  const char* name() const noexcept { return name_; }
  SEXP value() const noexcept { return value_; }

  // Comparison operators for efficient name-based comparison
  bool operator==(const named_arg& other) const noexcept {
    return std::strcmp(name_, other.name_) == 0;
  }

  bool operator!=(const named_arg& other) const noexcept { return !(*this == other); }

  bool operator==(const char* other_name) const noexcept {
    return std::strcmp(name_, other_name) == 0;
  }

  bool operator!=(const char* other_name) const noexcept {
    return std::strcmp(name_, other_name) != 0;
  }

 private:
  const char* name_;
  sexp value_;
};

namespace literals {

inline named_arg operator""_nm(const char* name, std::size_t) { return named_arg(name); }

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
  std::size_t operator()(const cpp4r::named_arg& arg) const noexcept {
    return std::hash<const char*>{}(arg.name());
  }
};
}  // namespace std
