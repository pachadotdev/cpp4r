/* roxygen
@title Square Coordinates (C++)
@param x integer vector
@description It squares each element of an integer vector.
@examples square_coordinates(1:3)
@export
*/
[[cpp4r::register]]
integers square_coordinates(integers x) {
  writable::integers out = x;
  for (int i = 0; i < x.size(); ++i) {
    out[i] = x[i] * x[i];
  }
  return out;
}

/* roxygen
@title Alternative Square Coordinates (C++)
@param x integer vector
@description It squares each element of an integer vector.
@examples square_coordinates_2(1:3)
@export
*/
[[cpp4r::register]]
integers square_coordinates_2(writable::integers x) {
  for (int i = 0; i < x.size(); ++i) {
    x[i] = x[i] * x[i];
  }
  return x;
}
