#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <cstring>      // for strcmp, strlen
#include <string>       // for string, basic_string, operator==
#include <type_traits>  // for is_convertible, enable_if
#include <utility>      // for move

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#include "R_ext/Memory.h"     // for vmaxget, vmaxset
#include "cpp4r/R.hpp"        // for SEXP, SEXPREC, Rf_mkCharCE, Rf_translat...
#include "cpp4r/as.hpp"       // for as_sexp
#include "cpp4r/protect.hpp"  // for unwind_protect, protect, protect::function
#include "cpp4r/sexp.hpp"     // for sexp

namespace cpp4r {

class r_string {
 public:
  r_string() = default;
  r_string(SEXP data) : data_(data) {}
  r_string(const char* data) : data_(safe[Rf_mkCharCE](data, CE_UTF8)) {}
  r_string(const std::string& data)
      : data_(safe[Rf_mkCharLenCE](data.c_str(), data.size(), CE_UTF8)) {}

#if CPP4R_HAS_CXX17
  // C++17: Constructor for std::string_view (avoids string copies)
  r_string(std::string_view data)
      : data_(safe[Rf_mkCharLenCE](data.data(), data.size(), CE_UTF8)) {}
#endif

  // Copy constructor
  r_string(const r_string& other) : data_(other.data_) {}

  // Copy assignment
  r_string& operator=(const r_string& other) {
    if (CPP4R_LIKELY(this != &other)) {
      data_ = other.data_;
    }
    return *this;
  }

  // Move constructor
  r_string(r_string&& other) noexcept : data_(other.data_) { other.data_ = R_NilValue; }

  // Move assignment
  r_string& operator=(r_string&& other) noexcept {
    if (CPP4R_LIKELY(this != &other)) {
      data_ = other.data_;
      other.data_ = R_NilValue;
    }
    return *this;
  }

  CPP4R_NODISCARD operator SEXP() const noexcept { return data_; }
  CPP4R_NODISCARD operator sexp() const noexcept { return data_; }
  CPP4R_NODISCARD operator std::string() const {
    std::string res;
    res.reserve(size());

    void* vmax = vmaxget();
    unwind_protect([&] { res.assign(Rf_translateCharUTF8(data_)); });
    vmaxset(vmax);

    return res;
  }

  CPP4R_NODISCARD bool operator==(const r_string& rhs) const noexcept {
    return data_.data() == rhs.data_.data();
  }

  CPP4R_NODISCARD bool operator==(const SEXP rhs) const noexcept {
    return data_.data() == rhs;
  }

  CPP4R_NODISCARD bool operator==(const char* rhs) const {
    return static_cast<std::string>(*this) == rhs;
  }

  CPP4R_NODISCARD bool operator==(const std::string& rhs) const {
    return static_cast<std::string>(*this) == rhs;
  }

#if CPP4R_HAS_CXX17
  // C++17: Comparison with std::string_view
  CPP4R_NODISCARD bool operator==(std::string_view rhs) const {
    return static_cast<std::string>(*this) == rhs;
  }
#endif

  CPP4R_NODISCARD R_xlen_t size() const noexcept { return Rf_xlength(data_); }

  // String search methods - optimized to minimize UTF-8 translation calls
  CPP4R_NODISCARD size_t find(const char* substr, size_t pos = 0) const {
    if (data_ == NA_STRING) {
      return std::string::npos;
    }
    // Note: Rf_translateCharUTF8 is expensive (encoding conversion)
    // For ASCII strings, CHAR() would be faster but less safe
    void* vmax = vmaxget();
    const char* str = Rf_translateCharUTF8(data_);
    const char* found = (str && substr) ? std::strstr(str + pos, substr) : nullptr;
    vmaxset(vmax);
    return found ? static_cast<size_t>(found - str) : std::string::npos;
  }

  CPP4R_NODISCARD size_t find(const std::string& substr, size_t pos = 0) const {
    return find(substr.c_str(), pos);
  }

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD size_t find(std::string_view substr, size_t pos = 0) const {
    return find(substr.data(), pos);
  }
#endif

  // Fast check for prefix without full string search
  CPP4R_NODISCARD bool starts_with(const char* prefix) const {
    if (data_ == NA_STRING || !prefix) {
      return false;
    }
    void* vmax = vmaxget();
    const char* str = Rf_translateCharUTF8(data_);
    bool result = str && (std::strncmp(str, prefix, std::strlen(prefix)) == 0);
    vmaxset(vmax);
    return result;
  }

 private:
  sexp data_ = R_NilValue;
};

inline SEXP as_sexp(std::initializer_list<r_string> il) {
  R_xlen_t size = il.size();

  sexp data;
  unwind_protect([&] {
    data = Rf_allocVector(STRSXP, size);
    auto it = il.begin();
    for (R_xlen_t i = 0; i < size; ++i, ++it) {
      if (CPP4R_UNLIKELY(*it == NA_STRING)) {
        SET_STRING_ELT(data, i, *it);
      } else {
        SET_STRING_ELT(data, i, Rf_mkCharCE(Rf_translateCharUTF8(*it), CE_UTF8));
      }
    }
  });
  return data;
}

template <typename T, typename R = void>
using enable_if_r_string = enable_if_t<std::is_same<T, cpp4r::r_string>::value, R>;

template <typename T>
enable_if_r_string<T, SEXP> as_sexp(T from) {
  r_string str(from);
  sexp res;
  unwind_protect([&] {
    res = Rf_allocVector(STRSXP, 1);

    if (CPP4R_UNLIKELY(str == NA_STRING)) {
      SET_STRING_ELT(res, 0, str);
    } else {
      SET_STRING_ELT(res, 0, Rf_mkCharCE(Rf_translateCharUTF8(str), CE_UTF8));
    }
  });

  return res;
}

template <>
CPP4R_NODISCARD inline r_string na() {
  return NA_STRING;
}

namespace traits {
template <>
struct get_underlying_type<r_string> {
  using type = SEXP;
};
}  // namespace traits

}  // namespace cpp4r
