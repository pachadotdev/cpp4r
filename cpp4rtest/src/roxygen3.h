// Test: documented + not documented + documented

/* roxygen
@title Roxygenise C++ function V
@param x numeric value
@description Dummy function to test roxygen2. It adds 5.0 to a double.
@export
@examples roxcpp5(1.0)
*/
[[cpp4r::register]] double roxcpp5(double x) {
  double y = x + 5.0;
  return y;
}

// Not Roxygenised C++ function VI
[[cpp4r::register]] double notroxcpp6_(double x) {
  double y = x + 6.0;
  return y;
}

/* roxygen
@title Roxygenise C++ function VII
@param x numeric value
@description Dummy function to test roxygen2. It adds 7.0 to a double.
@export
@examples
my_fun <- function(x) {
  roxcpp7(x)
}
@seealso \code{\link{roxcpp3}}
*/
[[cpp4r::register]] double roxcpp7(double x) {
  double y = x + 7.0;
  return y;
}
