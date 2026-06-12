/* roxygen
@title Release
@description Test suite
@param n number of objects to protect and release
@export
*/
[[cpp4r::register]] void release_(int n) {
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
