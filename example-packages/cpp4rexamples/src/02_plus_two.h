/* roxygen
@title Plus 2.0 (C++)
@param x double
@description It adds 2.0 to a double value.
@export
@examples plus_two(1.0)
*/
[[cpp4r::register]]
double plus_two(double x) {
  return x + 2.0;
}
