/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_for_(cpp4r::doubles x) {
  double sum = 0.;
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_for2_(SEXP x_sxp) {
  double sum = 0.;
  const cpp4r::doubles x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_for3_(SEXP x_sxp) {
  double sum = 0.;
  const cpp4r::writable::doubles x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_foreach_(cpp4r::doubles x) {
  double sum = 0.;
  for (const auto&& val : x) {
    sum += val;
  }
  return sum;
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_foreach2_(SEXP x_sxp) {
  const cpp4r::doubles x(x_sxp, false);
  double sum = 0.;
  for (const auto&& val : x) {
    sum += val;
  }
  return sum;
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_accumulate_(cpp4r::doubles x) {
  return std::accumulate(x.cbegin(), x.cend(), 0.);
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_dbl_accumulate2_(SEXP x_sxp) {
  const cpp4r::doubles x(x_sxp, false);
  return std::accumulate(x.cbegin(), x.cend(), 0.);
}

// Pacha: Functions for complex data type

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] cpp4r::r_complex sum_cplx_for_(cpp4r::complexes x) {
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] cpp4r::complexes sum_cplx_for_2_(cpp4r::complexes x) {
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] std::complex<double> sum_cplx_for_3_(cpp4r::complexes x_sxp) {
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] std::complex<double> sum_cplx_for_4_(SEXP x_sxp) {
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] SEXP sum_cplx_for_5_(SEXP x_sxp) {
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] cpp4r::complexes sum_cplx_for_6_(SEXP x_sxp) {
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] std::complex<double> sum_cplx_foreach_(cpp4r::complexes x) {
  std::complex<double> sum = {0.0, 0.0};
  for (const auto&& val : x) {
    // sum.real(sum.real() + val.real());
    // sum.imag(sum.imag() + val.imag());
    sum += val;
  }

  return sum;
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] std::complex<double> sum_cplx_accumulate_(cpp4r::complexes x) {
  return std::accumulate(x.cbegin(), x.cend(), std::complex<double>(0.0, 0.0));
}

/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] std::complex<double> sum_cplx_for2_(SEXP x_sxp) {
  std::complex<double> sum = {0.0, 0.0};
  const cpp4r::complexes x(x_sxp);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}
