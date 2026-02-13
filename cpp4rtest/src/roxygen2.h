// Test: documented + documented

/* roxygen
@title Roxygenise C++ function III
@param x numeric value
@description Dummy function to test roxygen2. It adds 3.0 to a double.
@export
@examples roxcpp3(1.0)
*/
[[cpp4r::register]] double roxcpp3(double x) {
  double y = x + 3.0;
  return y;
}

/* roxygen
@title Roxygenise C++ function IV
@param x numeric value
@description Dummy function to test roxygen2. It adds 4.0 to a double.
@export
@examples roxcpp4(1.0)
*/
[[cpp4r::register]] double roxcpp4(double x) {
  double y = x + 4.0;
  return y;
}
