/* roxygen
@title Sign of a number
@description Returns 1 if the number is positive, -1 if negative, and 0 if zero.
@param x A numeric value.
@return An integer indicating the sign of the number.
@examples
sign_cpp(10)
sign_cpp(-5)
sign_cpp(0)
@export
*/
[[cpp4r::register]] int sign_cpp(double x) {
  if (x > 0) {
    return 1;
  } else if (x == 0) {
    return 0;
  } else {
    return -1;
  }
}
