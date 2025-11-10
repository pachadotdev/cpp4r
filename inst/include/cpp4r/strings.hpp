#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <cstring>           // for strlen
#include <initializer_list>  // for initializer_list
#include <string>            // for string, basic_string

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#include "cpp4r/R.hpp"                // for SEXP, SEXPREC, SET_STRI...
#include "cpp4r/as.hpp"               // for as_sexp
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/named_arg.hpp"        // for named_arg
#include "cpp4r/protect.hpp"          // for safe
#include "cpp4r/r_string.hpp"         // for r_string
#include "cpp4r/r_vector.hpp"         // for r_vector, r_vector<>::proxy
#include "cpp4r/sexp.hpp"             // for sexp

// Specializations for strings

namespace cpp4r {

template <>
inline SEXPTYPE r_vector<r_string>::get_sexptype() {
  return STRSXP;
}

template <>
inline typename r_vector<r_string>::underlying_type r_vector<r_string>::get_elt(
    SEXP x, R_xlen_t i) {
  // NOPROTECT: likely too costly to unwind protect every elt
  return STRING_ELT(x, i);
}

template <>
inline typename r_vector<r_string>::underlying_type* r_vector<r_string>::get_p(
    bool, SEXP) noexcept {
  return nullptr;
}

template <>
inline typename r_vector<r_string>::underlying_type const*
r_vector<r_string>::get_const_p(bool is_altrep, SEXP data) {
  // No `STRING_PTR_OR_NULL()`
  if (is_altrep) {
    return nullptr;
  } else {
    return STRING_PTR_RO(data);
  }
}

template <>
inline void r_vector<r_string>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                           typename r_vector::underlying_type* buf) {
  cpp4r::stop("Unreachable!");
}

template <>
inline bool r_vector<r_string>::const_iterator::use_buf(bool is_altrep) {
  return false;
}

typedef r_vector<r_string> strings;

namespace writable {

template <>
inline void r_vector<r_string>::set_elt(
    SEXP x, R_xlen_t i, typename r_vector<r_string>::underlying_type value) {
  // NOPROTECT: Likely too costly to unwind protect every set elt
  SET_STRING_ELT(x, i, value);
}

// Pacha: Optimized push_back for std::string (borrows from @traversc' push_back_fast)
template <>
template <typename U, typename std::enable_if<std::is_same<U, r_string>::value>::type*>
inline void r_vector<r_string>::push_back(const std::string& value) {
  if (this->length_ >= this->capacity_) {
    // Calculate new capacity once instead of using while loop
    R_xlen_t new_capacity = this->capacity_ == 0 ? 1 : this->capacity_ * 2;
    this->reserve(new_capacity);
  }
  set_elt(this->data_, this->length_,
          Rf_mkCharLenCE(value.c_str(), value.size(), CE_UTF8));
  ++this->length_;
}

inline bool operator==(const r_vector<r_string>::proxy& lhs, r_string rhs) noexcept {
  return static_cast<r_string>(lhs).operator==(static_cast<std::string>(rhs).c_str());
}

inline SEXP alloc_or_copy(const SEXP data) {
  SEXPTYPE data_type = detail::r_typeof(data);
  if (data_type == STRSXP) {
    return safe[Rf_shallow_duplicate](data);
  } else if (data_type == CHARSXP) {
    return cpp4r::r_vector<r_string>(safe[Rf_allocVector](STRSXP, 1));
  } else {
    throw type_error(STRSXP, data_type);
  }
}

inline SEXP alloc_if_charsxp(const SEXP data) {
  SEXPTYPE data_type = detail::r_typeof(data);
  if (data_type == STRSXP) {
    return data;
  } else if (data_type == CHARSXP) {
    return cpp4r::r_vector<r_string>(safe[Rf_allocVector](STRSXP, 1));
  } else {
    throw type_error(STRSXP, data_type);
  }
}

template <>
inline r_vector<r_string>::r_vector(const SEXP& data)
    : cpp4r::r_vector<r_string>(alloc_or_copy(data)), capacity_(this->length_) {
  if (detail::r_typeof(data) == CHARSXP) {
    SET_STRING_ELT(this->data_, 0, data);
  }
}

template <>
inline r_vector<r_string>::r_vector(SEXP&& data)
    : cpp4r::r_vector<r_string>(alloc_if_charsxp(data)), capacity_(this->length_) {
  if (detail::r_typeof(data) == CHARSXP) {
    SET_STRING_ELT(this->data_, 0, data);
  }
}

// Requires specialization to handle `NA_STRING` and UTF-8 translation
template <>
inline r_vector<r_string>::r_vector(std::initializer_list<r_string> il)
    : cpp4r::r_vector<r_string>(safe[Rf_allocVector](STRSXP, il.size())),
      capacity_(il.size()) {
  unwind_protect([&] {
    auto it = il.begin();
    R_xlen_t i = 0;

    // Optimized loop: avoid repeated capacity checks and pre-increment iterator
    for (; i < this->capacity_ && it != il.end(); ++i, ++it) {
      // i.e. to `SEXP`
      typename r_vector<r_string>::underlying_type elt =
          static_cast<typename r_vector<r_string>::underlying_type>(*it);

      if (elt == NA_STRING) {
        SET_STRING_ELT(this->data_, i, elt);  // Direct access instead of set_elt
      } else {
        SET_STRING_ELT(this->data_, i, Rf_mkCharCE(Rf_translateCharUTF8(elt), CE_UTF8));
      }
    }
  });
}

typedef r_vector<r_string> strings;

template <typename T>
inline void r_vector<T>::push_back(const named_arg& value) {
  push_back(value.value());

  SEXP current_names = this->names();
  R_xlen_t current_size = this->size();

  if (Rf_xlength(current_names) == 0) {
    // Create names vector only once
    cpp4r::writable::strings new_nms(current_size);
    this->names() = new_nms;
    current_names = this->names();
  }

  // Direct access to avoid creating wrapper object
  // value.name() returns const char*, so use it directly
  const char* name_str = value.name();
  SET_STRING_ELT(current_names, current_size - 1,
                 Rf_mkCharLenCE(name_str, strlen(name_str), CE_UTF8));
}

}  // namespace writable

// Forward declarations for conversion functions
typedef r_vector<int> integers;
typedef r_vector<double> doubles;
typedef r_vector<r_bool> logicals;

// String conversion function - simpler is faster
inline strings as_strings(SEXP x) {
  SEXPTYPE x_type = detail::r_typeof(x);
  if (x_type == STRSXP) {
    return strings(x);
  }

  R_xlen_t len = Rf_length(x);
  if (len == 0) {
    return writable::strings(static_cast<R_xlen_t>(0));
  }

  if (x_type == CHARSXP) {
    writable::strings ret(static_cast<R_xlen_t>(1));
    SET_STRING_ELT(ret.data(), 0, x);
    return ret;
  }

  if (x_type == INTSXP) {
    integers xn(x);
    writable::strings ret(len);
    auto it = xn.begin();
    for (R_xlen_t i = 0; i < len; ++i, ++it) {
      int val = *it;
      if (val == NA_INTEGER) {
        SET_STRING_ELT(ret.data(), i, NA_STRING);
      } else {
        std::string str_val = std::to_string(val);
        SET_STRING_ELT(ret.data(), i,
                       Rf_mkCharLenCE(str_val.c_str(), str_val.size(), CE_UTF8));
      }
    }
    return ret;
  }

  if (x_type == REALSXP) {
    doubles xn(x);
    writable::strings ret(len);
    auto it = xn.begin();
    for (R_xlen_t i = 0; i < len; ++i, ++it) {
      double val = *it;
      if (ISNA(val)) {
        SET_STRING_ELT(ret.data(), i, NA_STRING);
      } else {
        std::string str_val = std::to_string(val);
        SET_STRING_ELT(ret.data(), i,
                       Rf_mkCharLenCE(str_val.c_str(), str_val.size(), CE_UTF8));
      }
    }
    return ret;
  }

  if (x_type == LGLSXP) {
    logicals xn(x);
    writable::strings ret(len);
    auto it = xn.begin();
    for (R_xlen_t i = 0; i < len; ++i, ++it) {
      r_bool val = *it;
      if (val == NA_LOGICAL) {
        SET_STRING_ELT(ret.data(), i, NA_STRING);
      } else if (static_cast<bool>(val)) {
        SET_STRING_ELT(ret.data(), i, Rf_mkCharCE("TRUE", CE_UTF8));
      } else {
        SET_STRING_ELT(ret.data(), i, Rf_mkCharCE("FALSE", CE_UTF8));
      }
    }
    return ret;
  }

  throw type_error(STRSXP, x_type);
}

}  // namespace cpp4r
