/* roxygen
@title Insert functions
@rdname testing-insert
@keywords internal
*/
[[cpp4r::register]] SEXP cpp4r_insert_(SEXP num_sxp) {
  R_xlen_t num = INTEGER(num_sxp)[0];

  R_xlen_t i = 0;
  cpp4r::writable::doubles out;
  while (i < num) {
    out.insert(0, i++);
  }
  return SEXP(out);
}
