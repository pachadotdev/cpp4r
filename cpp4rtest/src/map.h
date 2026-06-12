/* roxygen
@title Convert Ordered Map to List
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] SEXP ordered_map_to_list_(cpp4r::doubles x) {
  std::map<double, int> counts;
  int n = x.size();
  for (int i = 0; i < n; i++) {
    counts[x[i]]++;
  }
  return cpp4r::as_sexp(counts);
}

/* roxygen
@title Convert Unordered Map to List
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] SEXP unordered_map_to_list_(cpp4r::doubles x) {
  std::unordered_map<double, int> counts;
  int n = x.size();
  for (int i = 0; i < n; i++) {
    counts[x[i]]++;
  }
  return cpp4r::as_sexp(counts);
}
