/* roxygen
@title Sum Double Numbers on 'C++' Side (typed in, double out)
@description Test suite
@param x vector of double numbers (R)
@export
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
@title Sum Double Numbers on 'C++' Side (SEXP in, double out)
@description Test suite
@param x_sxp vector of double numbers (R)
@export
*/
[[cpp4r::register]] double sum_dbl_sexp_for_(SEXP x_sxp) {
  double sum = 0.;
  const cpp4r::doubles x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum Double Numbers on 'C++' Side (SEXP in no protect, double out)
@description Test suite
@param x_sxp vector of double numbers (R)
@export
*/
[[cpp4r::register]] double sum_dbl_sexp_writable_for_(SEXP x_sxp) {
  double sum = 0.;
  const cpp4r::writable::doubles x(x_sxp, false);
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum Double Numbers on 'C++' Side (typed in, double out)
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] double sum_dbl_foreach_(cpp4r::doubles x) {
  double sum = 0.;
  for (const auto& val : x) {
    sum += val;
  }
  return sum;
}

/* roxygen
@title Sum Double Numbers on 'C++' Side (SEXP in, double out)
@description Test suite
@param x_sxp vector of double numbers (R)
@export
*/
[[cpp4r::register]] double sum_dbl_sexp_foreach_(SEXP x_sxp) {
  const cpp4r::doubles x(x_sxp, false);
  double sum = 0.;
  for (const auto& val : x) {
    sum += val;
  }
  return sum;
}

/* roxygen
@title Sum Double Numbers on 'C++' Side (typed in, double out)
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] double sum_dbl_accumulate_(cpp4r::doubles x) {
  return std::accumulate(x.cbegin(), x.cend(), 0.);
}

/* roxygen
@title Sum Double Numbers on 'C++' Side (SEXP in, double out)
@description Test suite
@param x_sxp vector of double numbers (R)
@export
*/
[[cpp4r::register]] double sum_dbl_sexp_accumulate_(SEXP x_sxp) {
  const cpp4r::doubles x(x_sxp, false);
  return std::accumulate(x.cbegin(), x.cend(), 0.);
}
