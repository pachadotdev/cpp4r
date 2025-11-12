#pragma once

// Main r_vector header - includes all components

#include "cpp4r/r_vector_fwd.hpp"
#include "cpp4r/r_vector_impl.hpp"
#include "cpp4r/r_vector_writable_impl.hpp"

namespace cpp4r {

// Helper conversion functions

// Ensure that C is not constructible from SEXP, and neither C nor T is a std::string
template <typename C, typename T = typename std::decay<C>::type::value_type>
typename std::enable_if<
    !std::is_constructible<C, SEXP>::value &&
        !std::is_same<typename std::decay<C>::type, std::string>::value &&
        !std::is_same<typename std::decay<T>::type, std::string>::value,
    C>::type
as_cpp(SEXP from) {
  auto obj = cpp4r::r_vector<T>(from);
  return {obj.begin(), obj.end()};
}

// TODO: could we make this generalize outside of std::string?
template <typename C, typename T = typename std::decay<C>::type::value_type>
// typename T = typename C::value_type>
is_vector_of_strings<C, T> as_cpp(SEXP from) {
  auto obj = cpp4r::r_vector<cpp4r::r_string>(from);
  typename std::decay<C>::type res;
  auto it = obj.begin();
  while (it != obj.end()) {
    r_string s = *it;
    res.emplace_back(static_cast<std::string>(s));
    ++it;
  }
  return res;
}

// Comparison operators

template <typename T>
bool operator==(const r_vector<T>& lhs, const r_vector<T>& rhs) noexcept {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  auto lhs_it = lhs.begin();
  auto rhs_it = rhs.begin();

  auto end = lhs.end();
  while (lhs_it != end) {
    if (!(*lhs_it == *rhs_it)) {
      return false;
    }
    ++lhs_it;
    ++rhs_it;
  }
  return true;
}

template <typename T>
bool operator!=(const r_vector<T>& lhs, const r_vector<T>& rhs) noexcept {
  return !(lhs == rhs);
}

}  // namespace cpp4r
