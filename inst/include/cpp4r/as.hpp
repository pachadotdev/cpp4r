#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <cmath>  // for modf
#include <complex>
#include <initializer_list>  // for initializer_list
#include <map>               // for std::map
#include <memory>            // for std::shared_ptr, std::weak_ptr, std::unique_ptr
#include <stdexcept>
#include <string>         // for string, basic_string
#include <type_traits>    // for decay, enable_if, is_same, is_convertible
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#if CPP4R_HAS_STD_OPTIONAL
#include <optional>  // for std::optional (C++17)
#endif

#include "cpp4r/R.hpp"        // for SEXP, SEXPREC, Rf_xlength, R_xlen_t
#include "cpp4r/protect.hpp"  // for stop, protect, safe, protect::function

namespace cpp4r {

// C++14: Use variable templates for cleaner syntax
#if CPP4R_HAS_CXX14
template <bool C, typename R = void>
using enable_if_t = std::enable_if_t<C, R>;

template <typename T>
using decay_t = std::decay_t<T>;
#else
// C++11 fallback
template <bool C, typename R = void>
using enable_if_t = typename std::enable_if<C, R>::type;

template <typename T>
using decay_t = typename std::decay<T>::type;
#endif

template <typename T>
struct is_smart_ptr : std::false_type {};

template <typename T>
struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

template <typename T>
struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};

template <typename T>
struct is_smart_ptr<std::weak_ptr<T>> : std::true_type {};

template <typename T, typename R = void>
using enable_if_constructible_from_sexp =
    enable_if_t<!is_smart_ptr<T>::value &&  // workaround for gcc 4.8
                    std::is_class<T>::value && std::is_constructible<T, SEXP>::value,
                R>;

template <typename T, typename R = void>
using enable_if_is_sexp = enable_if_t<std::is_same<T, SEXP>::value, R>;

template <typename T, typename R = void>
using enable_if_convertible_to_sexp = enable_if_t<std::is_convertible<T, SEXP>::value, R>;

template <typename T, typename R = void>
using disable_if_convertible_to_sexp =
    enable_if_t<!std::is_convertible<T, SEXP>::value, R>;

template <typename T, typename R = void>
using enable_if_integral =
    enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value &&
                    !std::is_same<T, char>::value,
                R>;

template <typename T, typename R = void>
using enable_if_floating_point =
    typename std::enable_if<std::is_floating_point<T>::value, R>::type;

template <typename E, typename R = void>
using enable_if_enum = enable_if_t<std::is_enum<E>::value, R>;

template <typename T, typename R = void>
using enable_if_bool = enable_if_t<std::is_same<T, bool>::value, R>;

template <typename T, typename R = void>
using enable_if_char = enable_if_t<std::is_same<T, char>::value, R>;

template <typename T, typename R = void>
using enable_if_std_string = enable_if_t<std::is_same<T, std::string>::value, R>;

template <typename T, typename R = void>
using enable_if_c_string = enable_if_t<std::is_same<T, const char*>::value, R>;

// Detect std::complex types to avoid treating them as containers in generic
// container overloads.
template <typename>
struct is_std_complex : std::false_type {};

template <typename T>
struct is_std_complex<std::complex<T>> : std::true_type {};

// https://stackoverflow.com/a/1521682/2055486
//
#if CPP4R_HAS_CXX20
// C++20: Use constexpr for compile-time evaluation when possible
constexpr bool is_convertible_without_loss_to_integer(double value) noexcept {
  double int_part;
  return std::modf(value, &int_part) == 0.0;
}
#else
// C++11/14/17: Regular inline function
inline bool is_convertible_without_loss_to_integer(double value) noexcept {
  double int_part;
  return std::modf(value, &int_part) == 0.0;
}
#endif

template <typename T>
enable_if_constructible_from_sexp<T, T> as_cpp(SEXP from) {
  return T(from);
}

template <typename T>
enable_if_is_sexp<T, T> as_cpp(SEXP from) {
  return from;
}

template <typename T>
enable_if_integral<T, T> as_cpp(SEXP from) {
  // Check type first, then length - only pay length check cost for valid types
  if (Rf_isInteger(from)) {
    if (Rf_xlength(from) == 1) {
      return INTEGER_ELT(from, 0);
    }
  } else if (Rf_isReal(from)) {
    if (Rf_xlength(from) == 1) {
      if (ISNA(REAL_ELT(from, 0))) {
        return NA_INTEGER;
      }
      double value = REAL_ELT(from, 0);
      if (is_convertible_without_loss_to_integer(value)) {
        return value;
      }
    }
  } else if (Rf_isLogical(from)) {
    if (Rf_xlength(from) == 1) {
      if (LOGICAL_ELT(from, 0) == NA_LOGICAL) {
        return NA_INTEGER;
      }
    }
  }

  throw std::length_error("Expected single integer value");
}

template <typename E>
enable_if_enum<E, E> as_cpp(SEXP from) {
  if (Rf_isInteger(from)) {
#if CPP4R_HAS_CXX17
    // C++17: Use if constexpr for compile-time branch elimination
    using underlying_type = typename std::underlying_type<E>::type;
    if constexpr (std::is_same<char, underlying_type>::value) {
      return static_cast<E>(as_cpp<int>(from));
    } else {
      return static_cast<E>(as_cpp<underlying_type>(from));
    }
#else
    // C++11/14: Runtime conditional with type traits
    using underlying_type = typename std::underlying_type<E>::type;
    using int_type = typename std::conditional<std::is_same<char, underlying_type>::value,
                                               int,  // as_cpp<char> would trigger
                                                     // undesired string conversions
                                               underlying_type>::type;
    return static_cast<E>(as_cpp<int_type>(from));
#endif
  }

  throw std::length_error("Expected single integer value");
}

template <typename T>
enable_if_bool<T, T> as_cpp(SEXP from) {
  if (Rf_isLogical(from) && Rf_xlength(from) == 1) {
    return LOGICAL_ELT(from, 0) == 1;
  }

  throw std::length_error("Expected single logical value");
}

template <typename T>
enable_if_floating_point<T, T> as_cpp(SEXP from) {
  if (Rf_xlength(from) != 1) {
    throw std::length_error("Expected single floating point value");
  }

  if (Rf_isReal(from)) {
    return static_cast<T>(REAL_ELT(from, 0));
  }
  // All 32 bit integers can be coerced to doubles, so we just convert them.
  if (Rf_isInteger(from)) {
    if (INTEGER_ELT(from, 0) == NA_INTEGER) {
      return static_cast<T>(NA_REAL);
    }
    return static_cast<T>(INTEGER_ELT(from, 0));
  }

  // Also allow NA values
  if (Rf_isLogical(from)) {
    if (LOGICAL_ELT(from, 0) == NA_LOGICAL) {
      return static_cast<T>(NA_REAL);
    }
  }

  throw std::length_error("Expected single floating point value");
}

// Removed generic complex template to avoid ambiguity - use specific specializations
// instead

template <typename T>
enable_if_char<T, T> as_cpp(SEXP from) {
  if (Rf_isString(from) && Rf_xlength(from) == 1) {
    return unwind_protect([&] { return Rf_translateCharUTF8(STRING_ELT(from, 0))[0]; });
  }

  throw std::length_error("Expected string vector of length 1");
}

template <typename T>
enable_if_c_string<T, T> as_cpp(SEXP from) {
  if (Rf_isString(from) && Rf_xlength(from) == 1) {
    void* vmax = vmaxget();

    const char* result =
        unwind_protect([&] { return Rf_translateCharUTF8(STRING_ELT(from, 0)); });

    vmaxset(vmax);

    return {result};
  }

  throw std::length_error("Expected string vector of length 1");
}

template <typename T>
enable_if_std_string<T, T> as_cpp(SEXP from) {
  return {as_cpp<const char*>(from)};
}

// Specialization for converting std::complex<T> to SEXP
template <typename T>
inline SEXP as_sexp(const std::complex<T>& x) {
  SEXP result = PROTECT(Rf_allocVector(CPLXSXP, 1));
  COMPLEX(result)[0].r = static_cast<double>(x.real());
  COMPLEX(result)[0].i = static_cast<double>(x.imag());
  UNPROTECT(1);
  return result;
}

template <typename T>
enable_if_integral<T, SEXP> as_sexp(T from) noexcept {
  return safe[Rf_ScalarInteger](from);
}

template <typename T>
enable_if_floating_point<T, SEXP> as_sexp(T from) noexcept {
  return safe[Rf_ScalarReal](from);
}

template <typename T>
enable_if_bool<T, SEXP> as_sexp(T from) noexcept {
  return safe[Rf_ScalarLogical](from);
}

template <typename T>
enable_if_c_string<T, SEXP> as_sexp(T from) {
  return unwind_protect([&] { return Rf_ScalarString(Rf_mkCharCE(from, CE_UTF8)); });
}

template <typename T>
enable_if_std_string<T, SEXP> as_sexp(const T& from) {
  return as_sexp(from.c_str());
}

#if CPP4R_HAS_CXX17
// C++17: std::string_view overload
inline SEXP as_sexp(std::string_view from) {
  return unwind_protect(
      [&] { return Rf_ScalarString(Rf_mkCharLenCE(from.data(), from.size(), CE_UTF8)); });
}
#endif

template <typename Container, typename T = typename Container::value_type,
          typename = disable_if_convertible_to_sexp<Container>>
enable_if_integral<T, SEXP> as_sexp(const Container& from) {
  R_xlen_t size = from.size();
  SEXP data = safe[Rf_allocVector](INTSXP, size);

  auto it = from.begin();
  int* data_p = INTEGER(data);
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    data_p[i] = *it;
  }
  return data;
}

inline SEXP as_sexp(std::initializer_list<int> from) {
  return as_sexp<std::initializer_list<int>>(from);
}

template <typename Container, typename T = typename Container::value_type,
          typename = disable_if_convertible_to_sexp<Container>,
          typename = enable_if_t<!is_std_complex<Container>::value>>
enable_if_floating_point<T, SEXP> as_sexp(const Container& from) {
  R_xlen_t size = from.size();
  SEXP data = safe[Rf_allocVector](REALSXP, size);

  auto it = from.begin();
  double* data_p = REAL(data);
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    data_p[i] = *it;
  }
  return data;
}

inline SEXP as_sexp(std::initializer_list<double> from) {
  return as_sexp<std::initializer_list<double>>(from);
}

template <typename Container, typename T = typename Container::value_type,
          typename = disable_if_convertible_to_sexp<Container>,
          typename = enable_if_t<!is_std_complex<Container>::value>>
enable_if_bool<T, SEXP> as_sexp(const Container& from) {
  R_xlen_t size = from.size();
  SEXP data = safe[Rf_allocVector](LGLSXP, size);

  auto it = from.begin();
  int* data_p = LOGICAL(data);
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    data_p[i] = *it;
  }
  return data;
}

inline SEXP as_sexp(std::initializer_list<bool> from) {
  return as_sexp<std::initializer_list<bool>>(from);
}

namespace detail {
template <typename Container, typename AsCstring>
SEXP as_sexp_strings(const Container& from, AsCstring&& c_str) {
  R_xlen_t size = from.size();

  SEXP data = PROTECT(safe[Rf_allocVector](STRSXP, size));

  unwind_protect([&] {
    auto it = from.begin();
    for (R_xlen_t i = 0; i < size; ++i, ++it) {
      SET_STRING_ELT(data, i, Rf_mkCharCE(c_str(*it), CE_UTF8));
    }
  });

  UNPROTECT(1);
  return data;
}
}  // namespace detail

class r_string;

template <typename T, typename R = void>
using disable_if_r_string = enable_if_t<!std::is_same<T, cpp4r::r_string>::value, R>;

template <typename Container, typename T = typename Container::value_type,
          typename = disable_if_r_string<T>>
enable_if_t<std::is_convertible<T, std::string>::value &&
                !std::is_convertible<T, const char*>::value,
            SEXP>
as_sexp(const Container& from) {
  return detail::as_sexp_strings(from, [](const std::string& s) { return s.c_str(); });
}

template <typename Container, typename T = typename Container::value_type>
enable_if_c_string<T, SEXP> as_sexp(const Container& from) {
  return detail::as_sexp_strings(from, [](const char* s) { return s; });
}

inline SEXP as_sexp(std::initializer_list<const char*> from) {
  return as_sexp<std::initializer_list<const char*>>(from);
}

template <typename T, typename = disable_if_r_string<T>>
enable_if_convertible_to_sexp<T, SEXP> as_sexp(const T& from) {
  return from;
}

// Pacha: Specialization for std::map
// NOTE: I did not use templates to avoid clashes with doubles/function/etc.
inline SEXP as_sexp(const std::map<std::string, SEXP>& map) {
  R_xlen_t size = map.size();
  SEXP result = PROTECT(Rf_allocVector(VECSXP, size));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, size));

#if CPP4R_HAS_CXX17
  // C++17: Use structured bindings for cleaner iteration
  R_xlen_t i = 0;
  for (const auto& [key, value] : map) {
    SET_VECTOR_ELT(result, i, value);
    SET_STRING_ELT(names, i, Rf_mkCharCE(key.c_str(), CE_UTF8));
    ++i;
  }
#else
  // C++11/14: Traditional iterator approach
  auto it = map.begin();
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    SET_VECTOR_ELT(result, i, it->second);
    SET_STRING_ELT(names, i, Rf_mkCharCE(it->first.c_str(), CE_UTF8));
  }
#endif

  Rf_setAttrib(result, R_NamesSymbol, names);
  UNPROTECT(2);
  return result;
}

// Specialization for std::map<double, int>
inline SEXP as_sexp(const std::map<double, int>& map) {
  R_xlen_t size = map.size();
  SEXP result = PROTECT(Rf_allocVector(VECSXP, size));
  SEXP names = PROTECT(Rf_allocVector(REALSXP, size));

#if CPP4R_HAS_CXX17
  // C++17: Use structured bindings
  R_xlen_t i = 0;
  for (const auto& [key, value] : map) {
    SET_VECTOR_ELT(result, i, Rf_ScalarInteger(value));
    REAL(names)[i] = key;
    ++i;
  }
#else
  // C++11/14: Traditional iterator approach
  auto it = map.begin();
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    SET_VECTOR_ELT(result, i, Rf_ScalarInteger(it->second));
    REAL(names)[i] = it->first;
  }
#endif

  Rf_setAttrib(result, R_NamesSymbol, names);
  UNPROTECT(2);
  return result;
}

// Pacha: Specialization for std::unordered_map
inline SEXP as_sexp(const std::unordered_map<std::string, SEXP>& map) {
  R_xlen_t size = map.size();
  SEXP result = PROTECT(Rf_allocVector(VECSXP, size));
  SEXP names = PROTECT(Rf_allocVector(STRSXP, size));

#if CPP4R_HAS_CXX17
  // C++17: Use structured bindings
  R_xlen_t i = 0;
  for (const auto& [key, value] : map) {
    SET_VECTOR_ELT(result, i, value);
    SET_STRING_ELT(names, i, Rf_mkCharCE(key.c_str(), CE_UTF8));
    ++i;
  }
#else
  // C++11/14: Traditional iterator approach
  auto it = map.begin();
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    SET_VECTOR_ELT(result, i, it->second);
    SET_STRING_ELT(names, i, Rf_mkCharCE(it->first.c_str(), CE_UTF8));
  }
#endif

  Rf_setAttrib(result, R_NamesSymbol, names);
  UNPROTECT(2);
  return result;
}

// Specialization for std::unordered_map<double, int>
inline SEXP as_sexp(const std::unordered_map<double, int>& map) {
  R_xlen_t size = map.size();
  SEXP result = PROTECT(Rf_allocVector(VECSXP, size));
  SEXP names = PROTECT(Rf_allocVector(REALSXP, size));

#if CPP4R_HAS_CXX17
  // C++17: Use structured bindings
  R_xlen_t i = 0;
  for (const auto& [key, value] : map) {
    SET_VECTOR_ELT(result, i, Rf_ScalarInteger(value));
    REAL(names)[i] = key;
    ++i;
  }
#else
  // C++11/14: Traditional iterator approach
  auto it = map.begin();
  for (R_xlen_t i = 0; i < size; ++i, ++it) {
    SET_VECTOR_ELT(result, i, Rf_ScalarInteger(it->second));
    REAL(names)[i] = it->first;
  }
#endif

  Rf_setAttrib(result, R_NamesSymbol, names);
  UNPROTECT(2);
  return result;
}

}  // namespace cpp4r
