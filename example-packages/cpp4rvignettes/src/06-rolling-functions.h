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

/* roxygen
@title Cumulative Product
@description Returns a vector of doubles containing the minimum and maximum of a vector of doubles.
@param x doubles (numeric) vector.
@export
*/
[[cpp4r::register]]
doubles range_cpp(doubles x) {
  int n = x.size();
  double x1 = x[0], x2 = x[0];

  for (int i = 1; i < n; ++i) {
    x1 = std::min(x1, x[i]);
    x2 = std::max(x2, x[i]);
  }

  writable::doubles out(2);
  out[0] = x1;
  out[1] = x2;

  return out;
}
