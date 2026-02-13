// Test: not documented + documented

// Not Roxygenised C++ function I
[[cpp4r::register]] double notroxcpp1_(double x) {
  double y = x + 1.0;
  return y;
}

/* roxygen
@title Roxygenise C++ function II
@param x numeric value
@description Dummy function to test roxygen2. It adds 2.0 to a double.
@export
@examples roxcpp2(1.0)
*/
[[cpp4r::register]] double roxcpp2(double x) {
  double y = x + 2.0;
  return y;
}
