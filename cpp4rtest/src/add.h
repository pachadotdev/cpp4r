/* roxygen
@title Add functions
@rdname testing-add
@keywords internal
*/
[[cpp4r::register]] SEXP cpp4r_add_vec_for_(cpp4r::writable::doubles x, double num) {
  R_xlen_t n = x.size();
  for (R_xlen_t i = 0; i < n; ++i) {
    double cur = x[i];
    x[i] = cur + num;
  }

  return x;
}
