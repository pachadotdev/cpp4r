/* roxygen
@title Cumulative Sum
@description Returns a vector of doubles containing the cumulative sum of a vector of doubles.
@param x doubles (numeric) vector.
@export
*/
[[cpp4r::register]]
doubles cumsum_cpp(doubles x) {
  int n = x.size();
  writable::doubles out(n);

  out[0] = x[0];
  for (int i = 1; i < n; ++i) {
    out[i] = out[i - 1] + x[i];
  }
  return out;
}

/* roxygen
@title Cumulative Product
@description Returns a vector of doubles containing the cumulative product of a vector of doubles.
@param x doubles (numeric) vector.
@export
*/
[[cpp4r::register]] doubles cumprod_cpp(doubles x) {
  int n = x.size();
  writable::doubles out(n);

  out[0] = x[0];
  for (int i = 1; i < n; ++i) {
    out[i] = out[i - 1] * x[i];
  }
  return out;
}
