/* roxygen
@title Sum Integer Numbers on 'C++' Side (typed in, double out, for loop)
@description Test suite
@param x vector of integer numbers (R)
@export
*/
[[cpp4r::register]] double sum_int_for_(cpp4r::integers x) {
  double sum = 0.;
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }

  return sum;
}

/* roxygen
@title Sum Integer Numbers on 'C++' Side (SEXP in no protect, double out, for loop)
@description Test suite
@param x_sxp vector of integer numbers (R)
@export
*/
[[cpp4r::register]] double sum_int_sexp_for_(SEXP x_sxp) {
  cpp4r::integers x(x_sxp, false);

  double sum = 0.;
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    sum += x[i];
  }
  return sum;
}

/* roxygen
@title Sum Integer Numbers on 'C++' Side (typed in, double out, foreach)
@description Test suite
@param x vector of integer numbers (R)
@export
*/
[[cpp4r::register]] double sum_int_foreach_(cpp4r::integers x) {
  double sum = 0.;
  for (auto v : x) {
    sum += v;
  }

  return sum;
}

/* roxygen
@title Sum Integer Numbers on 'C++' Side (typed in, double out, accumulate)
@description Test suite
@param x vector of integer numbers (R)
@export
*/
[[cpp4r::register]] double sum_int_accumulate_(cpp4r::integers x) {
  return std::accumulate(x.cbegin(), x.cend(), 0.);
}
