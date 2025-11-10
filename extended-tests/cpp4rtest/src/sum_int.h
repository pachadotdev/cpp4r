/* roxygen
@title Sum functions
@rdname testing-sum
@keywords internal
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
@title Sum functions
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_int_for2_(SEXP x_) {
  cpp4r::integers x(x_, false);

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
[[cpp4r::register]] double sum_int_foreach_(cpp4r::integers x) {
  double sum = 0.;
  for (auto v : x) {
    sum += v;
  }

  return sum;
}

/* roxygen
@title Sum functionscpp4r
@rdname testing-sum
@keywords internal
*/
[[cpp4r::register]] double sum_int_accumulate_(cpp4r::integers x) {
  return std::accumulate(x.cbegin(), x.cend(), 0.);
}
