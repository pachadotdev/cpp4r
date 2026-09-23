/* roxygen
@title Plus 1 (C++)
@param x integer
@description It adds 1 to an integer value.
@export
@examples plus_one(1)
*/
[[cpp4r::register]]
int plus_one(int x) {
  return x + 1;
}
