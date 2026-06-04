/* roxygen
@title Sum Complex Numbers on 'C++' Side (typed in, r_complex out)
@description Test suite
@param x vector of complex numbers (R)
@export
*/
[[cpp4r::register]] cpp4r::r_complex sum_cplx_r_complex_out_(cpp4r::complexes x) {
  std::complex<double> sum = {0.0, 0.0};
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    // sum.real(sum.real() + x[i].real());
    // sum.imag(sum.imag() + x[i].imag());
    sum += x[i];
  }

  return cpp4r::r_complex(sum.real(), sum.imag());
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (typed in, complexes out)
@description Test suite
@param x vector of complex numbers (R)
@export
*/
[[cpp4r::register]] cpp4r::complexes sum_cplx_complexes_out_(cpp4r::complexes x) {
  std::complex<double> sum = {0.0, 0.0};
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    // sum.real(sum.real() + x[i].real());
    // sum.imag(sum.imag() + x[i].imag());
    sum += x[i];
  }

  cpp4r::writable::complexes result(1);
  // result[0] = cpp4r::r_complex(sum.real(), sum.imag());
  result[0] = sum;

  return result;
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (typed in re-wrapped, std out)
@description Test suite
@param x_sxp vector of complex numbers (R)
@export
*/
[[cpp4r::register]] std::complex<double> sum_cplx_typed_std_out_(cpp4r::complexes x_sxp) {
  std::complex<double> sum = {0.0, 0.0};
  const cpp4r::complexes x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    // sum.real(sum.real() + x[i].real());
    // sum.imag(sum.imag() + x[i].imag());
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (SEXP in no protect, std out)
@description Test suite
@param x_sxp vector of complex numbers (R)
@export
*/
[[cpp4r::register]] std::complex<double> sum_cplx_sexp_std_out_(SEXP x_sxp) {
  std::complex<double> sum = {0.0, 0.0};
  const cpp4r::complexes x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    // sum.real(sum.real() + x[i].real());
    // sum.imag(sum.imag() + x[i].imag());
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (SEXP in, SEXP out)
@description Test suite
@param x_sxp vector of complex numbers (R)
@export
*/
[[cpp4r::register]] SEXP sum_cplx_sexp_sexp_out_(SEXP x_sxp) {
  std::complex<double> sum = {0.0, 0.0};
  const cpp4r::complexes x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    // sum.real(sum.real() + x[i].real());
    // sum.imag(sum.imag() + x[i].imag());
    sum += x[i];
  }

  return cpp4r::as_sexp(sum);
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (SEXP in, complexes out)
@description Test suite
@param x_sxp vector of complex numbers (R)
@export
*/
[[cpp4r::register]] cpp4r::complexes sum_cplx_sexp_complexes_out_(SEXP x_sxp) {
  std::complex<double> sum = {0.0, 0.0};
  const cpp4r::complexes x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    // sum.real(sum.real() + x[i].real());
    // sum.imag(sum.imag() + x[i].imag());
    sum += x[i];
  }

  return cpp4r::as_sexp(sum);
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (foreach, std out)
@description Test suite
@param x vector of complex numbers (R)
@export
*/
[[cpp4r::register]] std::complex<double> sum_cplx_foreach_(cpp4r::complexes x) {
  std::complex<double> sum = {0.0, 0.0};
  for (const auto& val : x) {
    // sum.real(sum.real() + val.real());
    // sum.imag(sum.imag() + val.imag());
    sum += val;
  }

  return sum;
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (accumulate, std out)
@description Test suite
@param x vector of complex numbers (R)
@export
*/
[[cpp4r::register]] std::complex<double> sum_cplx_accumulate_(cpp4r::complexes x) {
  return std::accumulate(x.cbegin(), x.cend(), std::complex<double>(0.0, 0.0));
}

/* roxygen
@title Sum Complex Numbers on 'C++' Side (SEXP in with protect, std out)
@description Test suite
@param x_sxp vector of complex numbers (R)
@export
*/
[[cpp4r::register]] std::complex<double> sum_cplx_sexp_protect_std_out_(SEXP x_sxp) {
  std::complex<double> sum = {0.0, 0.0};
  const cpp4r::complexes x(x_sxp);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}
