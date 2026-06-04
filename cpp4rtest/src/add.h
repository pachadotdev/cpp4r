/* roxygen
@title Sum Doubles on 'C++' Side
@description Test suite
@param x vector of doubles (R)
@param num single double (C++)
@export
*/
[[cpp4r::register]] SEXP add_vec_for_(cpp4r::writable::doubles x, double num) {
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    double cur = x[i];
    x[i] = cur + num;
  }

  return x;
}
