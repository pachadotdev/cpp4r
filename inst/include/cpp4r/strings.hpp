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
r_vector<r_string>::get_const_p(bool is_altrep, SEXP data) noexcept {
  // No `STRING_PTR_OR_NULL()`
  if (CPP4R_UNLIKELY(is_altrep)) {
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
inline bool r_vector<r_string>::const_iterator::use_buf(bool is_altrep) noexcept {
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
  if (CPP4R_UNLIKELY(this->length_ >= this->capacity_)) {
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
  if (CPP4R_LIKELY(data_type == STRSXP)) {
    return safe[Rf_shallow_duplicate](data);
  } else if (CPP4R_UNLIKELY(data_type == CHARSXP)) {
    return cpp4r::r_vector<r_string>(safe[Rf_allocVector](STRSXP, 1));
  } else {
    throw type_error(STRSXP, data_type);
  }
}

inline SEXP alloc_if_charsxp(const SEXP data) {
  SEXPTYPE data_type = detail::r_typeof(data);
  if (CPP4R_LIKELY(data_type == STRSXP)) {
    return data;
  } else if (CPP4R_UNLIKELY(data_type == CHARSXP)) {
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

      if (CPP4R_UNLIKELY(elt == NA_STRING)) {
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

  if (CPP4R_UNLIKELY(Rf_xlength(current_names) == 0)) {
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

// Optimized as_strings conversion function
inline strings as_strings(SEXP x) {
  SEXPTYPE x_type = detail::r_typeof(x);
  if (CPP4R_LIKELY(x_type == STRSXP)) {
    return strings(x);
  }

  // Get length once and check for early exit
  R_xlen_t len = Rf_length(x);
  if (CPP4R_UNLIKELY(len == 0)) {
    return writable::strings(static_cast<R_xlen_t>(0));
  }

  if (CPP4R_UNLIKELY(x_type == CHARSXP)) {
    // Single character string - create a string vector with one element
    writable::strings ret(static_cast<R_xlen_t>(1));
    SET_STRING_ELT(ret.data(), 0, x);
    return ret;
  } else if (CPP4R_UNLIKELY(x_type == INTSXP)) {
    integers xn(x);
    writable::strings ret(len);

    // Use direct memory access when possible for better performance
    const int* src_ptr = INTEGER_OR_NULL(x);

    if (CPP4R_LIKELY(src_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(src_ptr[i] == NA_INTEGER)) {
          SET_STRING_ELT(ret.data(), i, NA_STRING);
        } else {
          std::string str_val = std::to_string(src_ptr[i]);
          SET_STRING_ELT(ret.data(), i,
                         Rf_mkCharLenCE(str_val.c_str(), str_val.size(), CE_UTF8));
        }
      }
    } else {
      // Fallback to iterator-based approach
      auto it = xn.begin();
      for (R_xlen_t i = 0; i < len; ++i, ++it) {
        int val = *it;
        if (CPP4R_UNLIKELY(val == NA_INTEGER)) {
          SET_STRING_ELT(ret.data(), i, NA_STRING);
        } else {
          std::string str_val = std::to_string(val);
          SET_STRING_ELT(ret.data(), i,
                         Rf_mkCharLenCE(str_val.c_str(), str_val.size(), CE_UTF8));
        }
      }
    }
    return ret;
  } else if (CPP4R_UNLIKELY(x_type == REALSXP)) {
    doubles xn(x);
    writable::strings ret(len);

    // Use direct memory access when possible
    const double* src_ptr = REAL_OR_NULL(x);

    if (CPP4R_LIKELY(src_ptr != nullptr)) {
      // Direct memory access - faster for large arrays
      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(ISNA(src_ptr[i]))) {
          SET_STRING_ELT(ret.data(), i, NA_STRING);
        } else {
          std::string str_val = std::to_string(src_ptr[i]);
          SET_STRING_ELT(ret.data(), i,
                         Rf_mkCharLenCE(str_val.c_str(), str_val.size(), CE_UTF8));
        }
      }
    } else {
      // Fallback to iterator-based approach
      auto it = xn.begin();
      for (R_xlen_t i = 0; i < len; ++i, ++it) {
        double val = *it;
        if (CPP4R_UNLIKELY(ISNA(val))) {
          SET_STRING_ELT(ret.data(), i, NA_STRING);
        } else {
          std::string str_val = std::to_string(val);
          SET_STRING_ELT(ret.data(), i,
                         Rf_mkCharLenCE(str_val.c_str(), str_val.size(), CE_UTF8));
        }
      }
    }
    return ret;
  } else if (CPP4R_UNLIKELY(x_type == LGLSXP)) {
    logicals xn(x);
    writable::strings ret(len);

    // Use direct memory access when possible
    const int* src_ptr = LOGICAL_OR_NULL(x);

    if (CPP4R_LIKELY(src_ptr != nullptr)) {
      // Direct memory access with cached string values
      static const char* true_str = "TRUE";
      static const char* false_str = "FALSE";

      for (R_xlen_t i = 0; i < len; ++i) {
        if (CPP4R_UNLIKELY(src_ptr[i] == NA_LOGICAL)) {
          SET_STRING_ELT(ret.data(), i, NA_STRING);
        } else if (src_ptr[i]) {
          SET_STRING_ELT(ret.data(), i, Rf_mkCharCE(true_str, CE_UTF8));
        } else {
          SET_STRING_ELT(ret.data(), i, Rf_mkCharCE(false_str, CE_UTF8));
        }
      }
    } else {
      // Fallback to iterator-based approach
      auto it = xn.begin();
      for (R_xlen_t i = 0; i < len; ++i, ++it) {
        r_bool val = *it;
        if (CPP4R_UNLIKELY(val == NA_LOGICAL)) {
          SET_STRING_ELT(ret.data(), i, NA_STRING);
        } else if (static_cast<bool>(val)) {
          SET_STRING_ELT(ret.data(), i, Rf_mkCharCE("TRUE", CE_UTF8));
        } else {
          SET_STRING_ELT(ret.data(), i, Rf_mkCharCE("FALSE", CE_UTF8));
        }
      }
    }
    return ret;
  }

  throw type_error(STRSXP, x_type);
}

// Optimized comparison operators for r_vector<r_string>
template <>
inline bool operator==(const r_vector<r_string>& lhs, const r_vector<r_string>& rhs) {
  if (CPP4R_UNLIKELY(lhs.size() != rhs.size())) return false;

  // Fast path: if both vectors point to the same data, they're equal
  if (CPP4R_UNLIKELY(lhs.data() == rhs.data())) return true;

  // Use direct element access when possible
  R_xlen_t len = lhs.size();
  for (R_xlen_t i = 0; i < len; ++i) {
    SEXP lhs_elt = STRING_ELT(lhs.data(), i);
    SEXP rhs_elt = STRING_ELT(rhs.data(), i);

    // Fast pointer comparison first
    if (CPP4R_LIKELY(lhs_elt == rhs_elt)) continue;

    // Handle NA cases
    if (CPP4R_UNLIKELY(lhs_elt == NA_STRING || rhs_elt == NA_STRING)) {
      if (lhs_elt != rhs_elt) return false;
      continue;
    }

    // String content comparison
    if (CPP4R_UNLIKELY(strcmp(CHAR(lhs_elt), CHAR(rhs_elt)) != 0)) return false;
  }

  return true;
}

template <>
inline bool operator!=(const r_vector<r_string>& lhs, const r_vector<r_string>& rhs) {
  return !(lhs == rhs);
}

// Optimized string utility functions
CPP4R_NODISCARD inline bool string_vector_contains_na(const r_vector<r_string>& vec) noexcept {
  R_xlen_t len = vec.size();
  for (R_xlen_t i = 0; i < len; ++i) {
    if (CPP4R_UNLIKELY(STRING_ELT(vec.data(), i) == NA_STRING)) {
      return true;
    }
  }
  return false;
}

CPP4R_NODISCARD inline R_xlen_t count_non_na_strings(const r_vector<r_string>& vec) noexcept {
  R_xlen_t count = 0;
  R_xlen_t len = vec.size();
  for (R_xlen_t i = 0; i < len; ++i) {
    if (CPP4R_LIKELY(STRING_ELT(vec.data(), i) != NA_STRING)) {
      ++count;
    }
  }
  return count;
}

// Fast string concatenation for two vectors of equal length
inline writable::strings string_paste(const r_vector<r_string>& lhs,
                                      const r_vector<r_string>& rhs,
                                      const std::string& sep = "") {
  R_xlen_t len = lhs.size();
  if (CPP4R_UNLIKELY(len != rhs.size())) {
    throw std::invalid_argument("Vector lengths must be equal for string concatenation");
  }

  writable::strings result(len);

  for (R_xlen_t i = 0; i < len; ++i) {
    SEXP lhs_elt = STRING_ELT(lhs.data(), i);
    SEXP rhs_elt = STRING_ELT(rhs.data(), i);

    if (CPP4R_UNLIKELY(lhs_elt == NA_STRING || rhs_elt == NA_STRING)) {
      SET_STRING_ELT(result.data(), i, NA_STRING);
    } else {
      std::string combined =
          std::string(CHAR(lhs_elt)) + sep + std::string(CHAR(rhs_elt));
      SET_STRING_ELT(result.data(), i,
                     Rf_mkCharLenCE(combined.c_str(), combined.size(), CE_UTF8));
    }
  }

  return result;
}

}  // namespace cpp4r
