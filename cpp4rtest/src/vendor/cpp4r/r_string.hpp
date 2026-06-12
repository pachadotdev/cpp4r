#pragma once

#include <string>
#include <type_traits>

// C++17+: std::string_view for zero-cost string argument passing
#if CPP4R_HAS_CXX17
#include <string_view>
#endif

#include "R_ext/Memory.h"
#include "cpp4r/R.hpp"
#include "cpp4r/as.hpp"
#include "cpp4r/protect.hpp"
#include "cpp4r/sexp.hpp"

namespace cpp4r {

class r_string {
 public:
  r_string() = default;
  r_string(SEXP data) noexcept : data_(data) {}
  r_string(const char* data) : data_(safe[Rf_mkCharCE](data, CE_UTF8)) {}
  r_string(const std::string& data)
      : data_(safe[Rf_mkCharLenCE](data.c_str(), data.size(), CE_UTF8)) {}

#if CPP4R_HAS_CXX17
  // C++17+: construct directly from a string_view, avoiding a temporary std::string
  r_string(std::string_view data)
      : data_(safe[Rf_mkCharLenCE](data.data(), static_cast<int>(data.size()), CE_UTF8)) {
  }
#endif

  CPP4R_ALWAYS_INLINE operator SEXP() const noexcept { return data_; }
  CPP4R_ALWAYS_INLINE operator sexp() const noexcept { return data_; }

  operator std::string() const {
    // `Rf_xlength(CHARSXP)` returns the byte length of the original string,
    // not the (possibly different) UTF-8 length, so don't `reserve` based on
    // it -- `assign` performs a single allocation sized correctly. The
    // `vmaxget`/`vmaxset` pair is required because `Rf_translateCharUTF8`
    // can use R's transient storage when re-encoding non-UTF-8 inputs.
    std::string res;
    void* vmax = vmaxget();
    unwind_protect([&] { res.assign(Rf_translateCharUTF8(data_)); });
    vmaxset(vmax);
    return res;
  }

  bool operator==(const r_string& rhs) const noexcept {
    return data_.data() == rhs.data_.data();
  }
  bool operator==(const SEXP rhs) const noexcept { return data_.data() == rhs; }
  bool operator==(const char* rhs) const {
#if CPP4R_HAS_CXX17
    void* vmax = vmaxget();
    const bool result = (std::string_view(Rf_translateCharUTF8(data_)) == rhs);
    vmaxset(vmax);
    return result;
#else
    return static_cast<std::string>(*this) == rhs;
#endif
  }
  bool operator==(const std::string& rhs) const {
#if CPP4R_HAS_CXX17
    void* vmax = vmaxget();
    const bool result = (std::string_view(Rf_translateCharUTF8(data_)) == rhs);
    vmaxset(vmax);
    return result;
#else
    return static_cast<std::string>(*this) == rhs;
#endif
  }

#if CPP4R_HAS_CXX17
  // C++17+: compare against a string_view without allocating a temporary std::string
  bool operator==(std::string_view rhs) const {
    void* vmax = vmaxget();
    const bool result = (std::string_view(Rf_translateCharUTF8(data_)) == rhs);
    vmaxset(vmax);
    return result;
  }
#endif

  CPP4R_NODISCARD R_xlen_t size() const noexcept { return Rf_xlength(data_); }

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
      if (*it == NA_STRING) {
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
    if (str == NA_STRING) {
      SET_STRING_ELT(res, 0, str);
    } else {
      SET_STRING_ELT(res, 0, Rf_mkCharCE(Rf_translateCharUTF8(str), CE_UTF8));
    }
  });
  return res;
}

template <>
inline r_string na() {
  return NA_STRING;
}

namespace traits {
template <>
struct get_underlying_type<r_string> {
  using type = SEXP;
};
}  // namespace traits

}  // namespace cpp4r
