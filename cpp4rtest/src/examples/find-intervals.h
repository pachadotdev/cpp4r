/* roxygen
@title Remove ALTREP from Vector on 'C++' Side
@description Test suite
@param x vector to process
@export
*/
[[cpp4r::register]] SEXP remove_altrep(SEXP x) {
  SEXP out = PROTECT(Rf_allocVector(REALSXP, Rf_xlength(x)));
  memcpy(REAL(out), REAL(x), Rf_xlength(x));

  UNPROTECT(1);
  return out;
}

/* roxygen
@title Find Interval for a Single Value on 'C++' Side (typed in, double out)
@description Test suite
@param x single value to find interval for
@param breaks vector of break points
@export
*/
[[cpp4r::register]] double upper_bound(doubles x, doubles breaks) {
  auto pos = std::upper_bound(breaks.begin(), breaks.end(), x[0]);
  return std::distance(breaks.begin(), pos);
}

/* roxygen
@title Find Interval for Multiple Values on 'C++' Side
@description Test suite
@param x vector of values to find intervals for
@param breaks vector of break points
@export
*/
[[cpp4r::register]] integers findInterval2(doubles x, doubles breaks) {
  writable::integers out(x.size());
  auto out_it = out.begin();

  for (auto&& val : x) {
    auto pos = std::upper_bound(breaks.begin(), breaks.end(), val);
    *out_it = std::distance(breaks.begin(), pos);
    ++out_it;
  }
  return out;
}

/* roxygen
@title Find Interval for Multiple Values on 'C++' Side (optimized)
@description Test suite
@param x vector of values to find intervals for
@param breaks vector of break points
@export
*/
[[cpp4r::register]] integers findInterval2_5(doubles x, doubles breaks) {
  writable::integers out(x.size());
  auto out_it = out.begin();
  auto bb = breaks.begin();
  auto be = breaks.end();

  for (auto&& val : x) {
    auto pos = std::upper_bound(bb, be, val);
    *out_it = std::distance(bb, pos);
    ++out_it;
  }
  return out;
}

// This version avoids the overhead of the cpp4r iterator types
/* roxygen
@title Find Interval for Multiple Values on 'C++' Side (optimized, raw pointers)
@description Test suite
@param x vector of values to find intervals for
@param breaks vector of break points
@export
*/
[[cpp4r::register]] integers findInterval3(doubles x, doubles breaks) {
  writable::integers out(x.size());
  auto out_it = out.begin();
  auto b = REAL(breaks);
  auto e = REAL(breaks) + Rf_xlength(breaks);
  double* pos;

  for (auto&& val : x) {
    pos = std::upper_bound(b, e, val);
    *out_it = std::distance(b, pos);
    ++out_it;
  }
  return out;
}

/* R code to benchmark these implementations
res <- bench::press(
  n1 = 10^seq(1, 3),
  n2 = 10^seq(1, 5),
  {
    x <- c(-n1, seq(-2, 2, length = n1 + 1), n1)
    y <- sort(round(stats::rt(n2, df = 2), 2))
    bench::mark(
      findInterval(x, y),
      findInterval2(x, y),
      findInterval2_5(x, y),
      findInterval3(x, y),
    )
  }
)
*/
