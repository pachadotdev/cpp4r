#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <complex>  // for std::complex
#include <utility>  // for move

#include "cpp4r/R.hpp"        // for SEXP, SEXPREC, Rf_mkCharCE, Rf_translateCharUTF8
#include "cpp4r/as.hpp"       // for as_sexp
#include "cpp4r/protect.hpp"  // for unwind_protect, protect, protect::function
#include "cpp4r/sexp.hpp"     // for sexp

namespace cpp4r {

class r_complex {
 public:
  r_complex() : data_(safe[Rf_allocVector](CPLXSXP, 1)) {
    Rcomplex* ptr = COMPLEX(data_);
    ptr[0].r = 0.0;
    ptr[0].i = 0.0;
  }
  r_complex(SEXP data) : data_(data) {
    if (CPP4R_UNLIKELY(data_ == R_NilValue)) {
      data_ = safe[Rf_allocVector](CPLXSXP, 0);
    }
  }
  r_complex(double real, double imag) : data_(safe[Rf_allocVector](CPLXSXP, 1)) {
    Rcomplex* ptr = COMPLEX(data_);
    ptr[0].r = real;
    ptr[0].i = imag;
  }
  r_complex(const std::complex<double>& data) : r_complex(data.real(), data.imag()) {}
  r_complex(const Rcomplex& data) : r_complex(data.r, data.i) {}

  // Copy constructor
  r_complex(const r_complex& other) : data_(other.data_) {}

  // Copy assignment
  r_complex& operator=(const r_complex& other) {
    if (CPP4R_LIKELY(this != &other)) {
      data_ = other.data_;
    }
    return *this;
  }

  // Move constructor
  r_complex(r_complex&& other) noexcept : data_(other.data_) { other.data_ = R_NilValue; }

  // Move assignment
  r_complex& operator=(r_complex&& other) noexcept {
    if (CPP4R_LIKELY(this != &other)) {
      data_ = other.data_;
      other.data_ = R_NilValue;
    }
    return *this;
  }

  CPP4R_NODISCARD operator SEXP() const noexcept { return data_; }
  CPP4R_NODISCARD operator sexp() const noexcept { return data_; }
  CPP4R_NODISCARD operator std::complex<double>() const {
    if (CPP4R_UNLIKELY(data_ == R_NilValue || Rf_length(data_) == 0)) {
      return {NA_REAL, NA_REAL};
    }
    return {COMPLEX(data_)[0].r, COMPLEX(data_)[0].i};
  }
  CPP4R_NODISCARD operator Rcomplex() const {
    Rcomplex r;
    if (CPP4R_UNLIKELY(data_ == R_NilValue || Rf_length(data_) == 0)) {
      r.r = NA_REAL;
      r.i = NA_REAL;
    } else {
      r.r = real();
      r.i = imag();
    }
    return r;
  }

  CPP4R_NODISCARD double real() const noexcept {
    if (CPP4R_UNLIKELY(data_ == R_NilValue || Rf_length(data_) == 0)) {
      return NA_REAL;
    }
    return COMPLEX(data_)[0].r;
  }
  CPP4R_NODISCARD double imag() const noexcept {
    if (CPP4R_UNLIKELY(data_ == R_NilValue || Rf_length(data_) == 0)) {
      return NA_REAL;
    }
    return COMPLEX(data_)[0].i;
  }

  bool operator==(const r_complex& rhs) const {
    return (is_na() && rhs.is_na()) || (real() == rhs.real() && imag() == rhs.imag());
  }

  bool operator!=(const r_complex& rhs) const { return !(*this == rhs); }

  r_complex& operator+=(const r_complex& rhs) {
    if (CPP4R_UNLIKELY(data_ == R_NilValue || Rf_length(data_) == 0 ||
                       rhs.data_ == R_NilValue || Rf_length(rhs.data_) == 0)) {
      *this = r_complex(real() + rhs.real(), imag() + rhs.imag());
    } else {
      COMPLEX(data_)[0].r += COMPLEX(rhs.data_)[0].r;
      COMPLEX(data_)[0].i += COMPLEX(rhs.data_)[0].i;
    }
    return *this;
  }

  r_complex& operator-=(const r_complex& rhs) {
    if (CPP4R_UNLIKELY(data_ == R_NilValue || Rf_length(data_) == 0 ||
                       rhs.data_ == R_NilValue || Rf_length(rhs.data_) == 0)) {
      *this = r_complex(real() - rhs.real(), imag() - rhs.imag());
    } else {
      COMPLEX(data_)[0].r -= COMPLEX(rhs.data_)[0].r;
      COMPLEX(data_)[0].i -= COMPLEX(rhs.data_)[0].i;
    }
    return *this;
  }

  r_complex& operator*=(const r_complex& rhs) {
    std::complex<double> lhs = *this;
    lhs *= static_cast<std::complex<double>>(rhs);
    *this = r_complex(lhs.real(), lhs.imag());
    return *this;
  }

  r_complex& operator/=(const r_complex& rhs) {
    std::complex<double> lhs = *this;
    lhs /= static_cast<std::complex<double>>(rhs);
    *this = r_complex(lhs.real(), lhs.imag());
    return *this;
  }

  friend r_complex operator+(r_complex lhs, const r_complex& rhs) {
    lhs += rhs;
    return lhs;
  }

  friend r_complex operator-(r_complex lhs, const r_complex& rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend r_complex operator*(r_complex lhs, const r_complex& rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend r_complex operator/(r_complex lhs, const r_complex& rhs) {
    lhs /= rhs;
    return lhs;
  }

  CPP4R_NODISCARD bool is_na() const { return R_IsNA(real()) || R_IsNA(imag()); }

 private:
  sexp data_ = R_NilValue;
};

inline SEXP as_sexp(const r_complex& from) {
  sexp res;
  unwind_protect([&] {
    res = Rf_allocVector(CPLXSXP, 1);
    COMPLEX(res)[0].r = from.real();
    COMPLEX(res)[0].i = from.imag();
  });

  return res;
}

inline SEXP as_sexp(std::initializer_list<r_complex> il) {
  R_xlen_t size = il.size();

  sexp data;
  unwind_protect([&] {
    data = Rf_allocVector(CPLXSXP, size);
    auto it = il.begin();
    for (R_xlen_t i = 0; i < size; ++i, ++it) {
      COMPLEX(data)[i].r = it->real();
      COMPLEX(data)[i].i = it->imag();
    }
  });
  return data;
}

template <>
CPP4R_NODISCARD inline r_complex na() {
  return r_complex(NA_REAL, NA_REAL);
}

// Specialized is_na for r_complex
CPP4R_NODISCARD inline bool is_na(const r_complex& value) noexcept {
  return value.is_na();
}

namespace traits {
template <>
struct get_underlying_type<r_complex> {
  using type = Rcomplex;
};
}  // namespace traits

}  // namespace cpp4r
