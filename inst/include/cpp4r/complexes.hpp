#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <algorithm>         // for std::transform
#include <complex>           // for std::complex
#include <initializer_list>  // for std::initializer_list

#include "cpp4r/R.hpp"  // for SEXP, SEXPREC, Rf_allocVector, COMPLEX, COMPLEX_ELT, SET_COMPLEX_ELT
#include "cpp4r/as.hpp"         // for as_sexp
#include "cpp4r/protect.hpp"    // for safe
#include "cpp4r/r_complex.hpp"  // for r_complex
#include "cpp4r/r_vector.hpp"   // for r_vector, r_vector<>::proxy
#include "cpp4r/sexp.hpp"       // for sexp

namespace cpp4r {

// Specializations for complex numbers

template <>
inline SEXPTYPE r_vector<r_complex>::get_sexptype() {
  return CPLXSXP;
}

template <>
inline typename r_vector<r_complex>::underlying_type r_vector<r_complex>::get_elt(
    SEXP x, R_xlen_t i) {
  return COMPLEX_ELT(x, i);
}

template <>
inline typename r_vector<r_complex>::underlying_type* r_vector<r_complex>::get_p(
    bool is_altrep, SEXP data) noexcept {
#if CPP4R_HAS_CXX20
  if (CPP4R_UNLIKELY(is_altrep)) {
    return nullptr;
  } else {
    return COMPLEX(data);
  }
#else
  if (__builtin_expect(is_altrep, 0)) {
    return nullptr;
  } else {
    return COMPLEX(data);
  }
#endif
}

template <>
inline typename r_vector<r_complex>::underlying_type const*
r_vector<r_complex>::get_const_p(bool is_altrep, SEXP data) noexcept {
  return COMPLEX_OR_NULL(data);
}

template <>
inline void r_vector<r_complex>::get_region(SEXP x, R_xlen_t i, R_xlen_t n,
                                            typename r_vector::underlying_type* buf) {
  COMPLEX_GET_REGION(x, i, n, buf);
}

template <>
inline bool r_vector<r_complex>::const_iterator::use_buf(bool is_altrep) noexcept {
  return __builtin_expect(is_altrep, 0);
}

typedef r_vector<r_complex> complexes;

namespace writable {

template <>
inline void r_vector<r_complex>::set_elt(
    SEXP x, R_xlen_t i, typename cpp4r::r_vector<r_complex>::underlying_type value) {
  COMPLEX(x)[i] = value;
}

typedef r_vector<r_complex> complexes;

}  // namespace writable

inline complexes as_complexes(SEXP x) {
#if CPP4R_HAS_CXX20
  if (CPP4R_LIKELY(detail::r_typeof(x) == CPLXSXP)) {
    return complexes(x);
  }

  else if (CPP4R_UNLIKELY(detail::r_typeof(x) == INTSXP)) {
#else
  if (__builtin_expect(detail::r_typeof(x) == CPLXSXP, 1)) {
    return complexes(x);
  }

  else if (__builtin_expect(detail::r_typeof(x) == INTSXP, 0)) {
#endif
    r_vector<int> xn(x);
    size_t len = xn.size();
    writable::complexes ret(len);
    std::transform(xn.begin(), xn.end(), ret.begin(), [](int value) noexcept {
      return __builtin_expect(value == NA_INTEGER, 0)
                 ? r_complex(NA_REAL, NA_REAL)
                 : r_complex(static_cast<double>(value), 0.0);
    });
    return ret;
  }

  throw type_error(CPLXSXP, detail::r_typeof(x));
}

// Define comparison operators within the proxy class
namespace writable {

template <>
class r_vector<r_complex>::proxy {
 public:
  proxy(SEXP data, R_xlen_t index) noexcept
      : data_(data), index_(index), buf_(nullptr), is_altrep_(false) {}

  proxy(SEXP data, R_xlen_t index, Rcomplex* buf, bool is_altrep) noexcept
      : data_(data), index_(index), buf_(buf), is_altrep_(is_altrep) {}

#if CPP4R_HAS_CXX20
  operator r_complex() const noexcept {
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      return r_complex(buf_->r, buf_->i);
    } else {
      Rcomplex r = COMPLEX_ELT(data_, index_);
      return r_complex(r.r, r.i);
    }
  }
#else
  operator r_complex() const noexcept {
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      return r_complex(buf_->r, buf_->i);
    } else {
      Rcomplex r = COMPLEX_ELT(data_, index_);
      return r_complex(r.r, r.i);
    }
  }
#endif

  proxy& operator=(const r_complex& value) noexcept {
#if CPP4R_HAS_CXX20
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      buf_->r = value.real();
      buf_->i = value.imag();
    } else {
      Rcomplex r;
      r.r = value.real();
      r.i = value.imag();
      SET_COMPLEX_ELT(data_, index_, r);
    }
#else
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      buf_->r = value.real();
      buf_->i = value.imag();
    } else {
      Rcomplex r;
      r.r = value.real();
      r.i = value.imag();
      SET_COMPLEX_ELT(data_, index_, r);
    }
#endif
    return *this;
  }

  proxy& operator=(const std::complex<double>& value) noexcept {
#if CPP4R_HAS_CXX20
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      buf_->r = value.real();
      buf_->i = value.imag();
    } else {
      Rcomplex r;
      r.r = value.real();
      r.i = value.imag();
      SET_COMPLEX_ELT(data_, index_, r);
    }
#else
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      buf_->r = value.real();
      buf_->i = value.imag();
    } else {
      Rcomplex r;
      r.r = value.real();
      r.i = value.imag();
      SET_COMPLEX_ELT(data_, index_, r);
    }
#endif
    return *this;
  }

  proxy& operator+=(const r_complex& value) noexcept {
    // Direct arithmetic on components to avoid temporary objects
#if CPP4R_HAS_CXX20
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      buf_->r += value.real();
      buf_->i += value.imag();
    } else {
      Rcomplex current = COMPLEX_ELT(data_, index_);
      current.r += value.real();
      current.i += value.imag();
      SET_COMPLEX_ELT(data_, index_, current);
    }
#else
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      buf_->r += value.real();
      buf_->i += value.imag();
    } else {
      Rcomplex current = COMPLEX_ELT(data_, index_);
      current.r += value.real();
      current.i += value.imag();
      SET_COMPLEX_ELT(data_, index_, current);
    }
#endif
    return *this;
  }

  proxy& operator-=(const r_complex& value) noexcept {
    // Direct arithmetic on components to avoid temporary objects
#if CPP4R_HAS_CXX20
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      buf_->r -= value.real();
      buf_->i -= value.imag();
    } else {
      Rcomplex current = COMPLEX_ELT(data_, index_);
      current.r -= value.real();
      current.i -= value.imag();
      SET_COMPLEX_ELT(data_, index_, current);
    }
#else
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      buf_->r -= value.real();
      buf_->i -= value.imag();
    } else {
      Rcomplex current = COMPLEX_ELT(data_, index_);
      current.r -= value.real();
      current.i -= value.imag();
      SET_COMPLEX_ELT(data_, index_, current);
    }
#endif
    return *this;
  }

  proxy& operator*=(const r_complex& value) noexcept {
    // Complex multiplication: (a+bi)(c+di) = (ac-bd) + (ad+bc)i
#if CPP4R_HAS_CXX20
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      double real_part = buf_->r * value.real() - buf_->i * value.imag();
      double imag_part = buf_->r * value.imag() + buf_->i * value.real();
      buf_->r = real_part;
      buf_->i = imag_part;
    } else {
      Rcomplex current = COMPLEX_ELT(data_, index_);
      double real_part = current.r * value.real() - current.i * value.imag();
      double imag_part = current.r * value.imag() + current.i * value.real();
      current.r = real_part;
      current.i = imag_part;
      SET_COMPLEX_ELT(data_, index_, current);
    }
#else
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      double real_part = buf_->r * value.real() - buf_->i * value.imag();
      double imag_part = buf_->r * value.imag() + buf_->i * value.real();
      buf_->r = real_part;
      buf_->i = imag_part;
    } else {
      Rcomplex current = COMPLEX_ELT(data_, index_);
      double real_part = current.r * value.real() - current.i * value.imag();
      double imag_part = current.r * value.imag() + current.i * value.real();
      current.r = real_part;
      current.i = imag_part;
      SET_COMPLEX_ELT(data_, index_, current);
    }
#endif
    return *this;
  }

  proxy& operator/=(const r_complex& value) noexcept {
    // Complex division: (a+bi)/(c+di) = ((ac+bd) + (bc-ad)i)/(c²+d²)
    const double c = value.real();
    const double d = value.imag();
    const double denom_inv = 1.0 / (c * c + d * d);  // Compute reciprocal once

#if CPP4R_HAS_CXX20
    if (CPP4R_UNLIKELY(is_altrep_ && buf_ != nullptr)) {
      const double a = buf_->r;
      const double b = buf_->i;
      buf_->r = (a * c + b * d) * denom_inv;
      buf_->i = (b * c - a * d) * denom_inv;
    } else {
      const Rcomplex current = COMPLEX_ELT(data_, index_);
      const double a = current.r;
      const double b = current.i;
      Rcomplex result;
      result.r = (a * c + b * d) * denom_inv;
      result.i = (b * c - a * d) * denom_inv;
      SET_COMPLEX_ELT(data_, index_, result);
    }
#else
    if (__builtin_expect(is_altrep_ && buf_ != nullptr, 0)) {
      const double a = buf_->r;
      const double b = buf_->i;
      buf_->r = (a * c + b * d) * denom_inv;
      buf_->i = (b * c - a * d) * denom_inv;
    } else {
      const Rcomplex current = COMPLEX_ELT(data_, index_);
      const double a = current.r;
      const double b = current.i;
      Rcomplex result;
      result.r = (a * c + b * d) * denom_inv;
      result.i = (b * c - a * d) * denom_inv;
      SET_COMPLEX_ELT(data_, index_, result);
    }
#endif
    return *this;
  }

  friend bool operator==(const proxy& lhs, const r_complex& rhs) {
    return static_cast<r_complex>(lhs) == rhs;
  }

  friend bool operator!=(const proxy& lhs, const r_complex& rhs) { return !(lhs == rhs); }

 private:
  SEXP data_;
  R_xlen_t index_;
  Rcomplex* buf_;
  bool is_altrep_;
};

}  // namespace writable

// New complexes_vector class for handling complex numbers in SEXP
class complexes_vector {
 public:
#if CPP4R_HAS_CXX17
  // C++17: Mark constructor as [[nodiscard]] candidate
  CPP4R_NODISCARD explicit complexes_vector(SEXP x) noexcept
      : data_(reinterpret_cast<Rcomplex*>(DATAPTR(x))), size_(Rf_length(x)) {}
#else
  explicit complexes_vector(SEXP x) noexcept
      : data_(reinterpret_cast<Rcomplex*>(DATAPTR(x))), size_(Rf_length(x)) {}
#endif

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD std::complex<double> operator[](R_xlen_t i) const noexcept {
    return {data_[i].r, data_[i].i};
  }

  CPP4R_NODISCARD size_t size() const noexcept { return size_; }
#else
  std::complex<double> operator[](R_xlen_t i) const noexcept {
    return {data_[i].r, data_[i].i};
  }

  size_t size() const noexcept { return size_; }
#endif

 private:
  Rcomplex* data_;
  size_t size_;
};

// Template specialization for adding cpp4r::r_complex to std::complex<double>
template <typename T>
inline std::complex<T>& operator+=(std::complex<T>& lhs, const cpp4r::r_complex& rhs) {
#if CPP4R_HAS_CXX17
  // C++17: More efficient with guaranteed copy elision and structured initialization
  T new_real = lhs.real() + static_cast<T>(rhs.real());
  T new_imag = lhs.imag() + static_cast<T>(rhs.imag());
  lhs = std::complex<T>(new_real, new_imag);
#else
  // C++11/14: Standard approach
  T new_real = lhs.real() + static_cast<T>(rhs.real());
  T new_imag = lhs.imag() + static_cast<T>(rhs.imag());
  lhs = std::complex<T>(new_real, new_imag);
#endif
  return lhs;
}

// Add constructor for initializer_list for the writable r_vector specialization
namespace writable {

template <>
inline r_vector<r_complex>::r_vector(std::initializer_list<r_complex> il)
    : cpp4r::r_vector<r_complex>(safe[Rf_allocVector](CPLXSXP, il.size())),
      capacity_(il.size()) {
  auto it = il.begin();

  if (data_p_ != nullptr) {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      data_p_[i] = static_cast<underlying_type>(*it);
    }
  } else {
    for (R_xlen_t i = 0; i < capacity_; ++i, ++it) {
      set_elt(data_, i, static_cast<underlying_type>(*it));
    }
  }
}

}  // namespace writable

// Comparison operators for r_vector<r_complex>
template <>
inline bool operator==(const r_vector<r_complex>& lhs, const r_vector<r_complex>& rhs) {
#if CPP4R_HAS_CXX20
  if (lhs.size() != rhs.size()) CPP4R_UNLIKELY return false;

  // Fast path: if both vectors point to the same data, they're equal
  if (lhs.data() == rhs.data()) CPP4R_LIKELY return true;
#else
  if (lhs.size() != rhs.size()) return false;

  // Fast path: if both vectors point to the same data, they're equal
  if (lhs.data() == rhs.data()) return true;
#endif

  // Use iterators for potentially better performance
  auto lhs_it = lhs.cbegin();
  auto rhs_it = rhs.cbegin();
  auto lhs_end = lhs.cend();

  for (; lhs_it != lhs_end; ++lhs_it, ++rhs_it) {
    if (!(*lhs_it == *rhs_it)) return false;
  }
  return true;
}

template <>
inline bool operator!=(const r_vector<r_complex>& lhs, const r_vector<r_complex>& rhs) {
  return !(lhs == rhs);
}

}  // namespace cpp4r
