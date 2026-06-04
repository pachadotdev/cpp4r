/* roxygen
@title Push and Truncate Doubles
@description Test suite
@param size_sexp initial size of the vector to grow and truncate
@export
*/
[[cpp4r::register]] SEXP push_and_truncate_(SEXP size_sexp) {
  R_xlen_t size = INTEGER(size_sexp)[0];

  // Allocate `size` worth of doubles (filled with garbage data)
  cpp4r::writable::doubles out(size);

  // Push 1 more past the existing length/capacity,
  // doubling the capacity for cpp4r vectors
  out.push_back(0);

  // Truncate back to `size + 1` size and return result.
  return SEXP(out);
}
