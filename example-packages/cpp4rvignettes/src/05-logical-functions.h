/* roxygen
@title Is Any Element in a Logical Vector "TRUE"
@description Loop over a vector and returns "true" when a "true" element is found in the sequence.
@param x logical vector.
@export
*/
[[cpp4r::register]]
bool any_cpp(logicals x) {
  int n = x.size();
  
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      return true;
    }
  }
  return false;
}

/* roxygen
@title Which Elements in a Logical Vector are "TRUE"
@description Loop over a vector and returns the positions that are "true". Returns a vector of
  integers.
@param x logical vector.
@export
*/
[[cpp4r::register]]
integers which_cpp(logicals x) {
  int n = x.size();
  writable::integers res;
  int j = 0;

  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      ++j;
      res.push_back(i + 1);
    }
  }

  if (j == 0) {
    return integers(0);
  } else {
    return res;
  }
}

/* roxygen
@title Are all Elements in a Vector "TRUE"? (Form 1)
@description Loop over a vector and returns "true" if all the elements are "true".
@param x logical vector.
@export
*/
[[cpp4r::register]]
bool all_cpp_1(logicals x) {
  int n = x.size();
  for (int i = 0; i < n; ++i) {
    if (!x[i]) {
      return false;
    }
  }
  return true;
}

/* roxygen
@title Are all Elements in a Vector "TRUE"? (Form 2)
@description Loop over a vector and returns "true" if all the elements are "true".
@param x logical vector.
@export
*/
[[cpp4r::register]]
bool all_cpp_2(logicals x) {
  for (int i = 0; i < x.size(); ++i) {
    if (!x[i]) {
      return false;
    }
  }
  return true;
}

/* roxygen
@title Are all Elements in a Vector "TRUE"? (Form 3)
@description Loop over a vector and returns "true" if all the elements are "true".
@param x logical vector.
@export
*/
[[cpp4r::register]] bool
all_cpp_3(logicals x) {
  for (bool i : x) {
    if (!i) {
      return false;
    }
  }
  return true;
}

/* roxygen
@title Are all Elements in a Vector "TRUE"? (Form 4)
@description Loop over a vector and returns "true" if all the elements are "true".
@param x logical vector.
@export
*/
[[cpp4r::register]] bool
all_cpp_4(logicals x) {
  return std::all_of(x.begin(), x.end(), [](bool x) { return x; });
}
