/* roxygen
@title Release functions
@rdname testing-release
@keywords internal
*/
[[cpp4r::register]] void cpp4r_release_(int n) {
  std::vector<cpp4r::sexp> x;
  int count = 0;
  while (count < n) {
    x.push_back(Rf_ScalarInteger(count));
    ++count;
  }
  count = 0;
  while (count < n) {
    x.pop_back();
    ++count;
  }
}
