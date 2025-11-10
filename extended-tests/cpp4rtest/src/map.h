/* roxygen
@title List functions for cpp4r
@rdname testing-map
@keywords internal
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
@title List functions for cpp4r
@rdname testing-map
@keywords internal
*/
[[cpp4r::register]] SEXP unordered_map_to_list_(cpp4r::doubles x) {
  std::unordered_map<double, int> counts;
  int n = x.size();
  for (int i = 0; i < n; i++) {
    counts[x[i]]++;
  }
  return cpp4r::as_sexp(counts);
}
