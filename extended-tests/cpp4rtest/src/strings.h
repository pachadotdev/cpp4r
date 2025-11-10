// issue 406

#include <R_ext/Random.h>

void set_seed(int seed) {
  // Use R's set.seed() by evaluating it
  SEXP call = PROTECT(Rf_lang2(Rf_install("set.seed"), Rf_ScalarInteger(seed)));
  Rf_eval(call, R_GlobalEnv);
  UNPROTECT(1);
}

double random_double() {
  GetRNGstate();
  double result = unif_rand();
  PutRNGstate();
  return result;
}

int random_int(int min, int max) {
  GetRNGstate();
  int result = min + static_cast<int>((max - min + 1) * unif_rand());
  PutRNGstate();
  return result;
}

std::string random_string() {
  std::string s(10, '\0');
  GetRNGstate();
  for (size_t i = 0; i < 10; i++) {
    s[i] = static_cast<char>('a' + static_cast<int>(26 * unif_rand()));
  }
  PutRNGstate();
  return s;
}

/* roxygen
@title Grow functions
@rdname testing-grow-strings
@keywords internal
*/
[[cpp4r::register]] cpp4r::strings grow_strings_cpp4r_(size_t n, int seed) {
  set_seed(seed);
  cpp4r::writable::strings x;
  for (size_t i = 0; i < n; ++i) {
    x.push_back(random_string());
  }
  return x;
}

/* roxygen
@title Grow functions
@rdname testing-grow-strings
@keywords internal
*/
[[cpp4r::register]] SEXP grow_strings_manual_(size_t n, int seed) {
  set_seed(seed);
  SEXP data_ = PROTECT(Rf_allocVector(STRSXP, 0));
  size_t size_ = 0;
  size_t capacity_ = 0;
  for (size_t i = 0; i < n; ++i) {
    if (size_ == capacity_) {
      capacity_ = capacity_ == 0 ? 1 : capacity_ * 2;
      SEXP new_data_ = PROTECT(Rf_allocVector(STRSXP, capacity_));
      for (size_t j = 0; j < size_; ++j) {
        SET_STRING_ELT(new_data_, j, STRING_ELT(data_, j));
      }
      UNPROTECT(2);
      data_ = PROTECT(new_data_);
    }
    SET_STRING_ELT(data_, size_++, Rf_mkChar(random_string().c_str()));
  }
  // copy back down to size
  if (size_ < capacity_) {
    SEXP new_data_ = PROTECT(Rf_allocVector(STRSXP, size_));
    for (size_t j = 0; j < size_; ++j) {
      SET_STRING_ELT(new_data_, j, STRING_ELT(data_, j));
    }
    UNPROTECT(2);
    return new_data_;
  } else {
    UNPROTECT(1);
    return data_;
  }
}

/* roxygen
@title Grow functions
@rdname testing-grow-strings
@keywords internal
*/
[[cpp4r::register]] cpp4r::strings assign_cpp4r_(size_t n, int seed) {
  set_seed(seed);
  cpp4r::writable::strings x(n);
  for (size_t i = 0; i < n; ++i) {
    x[i] = random_string();
  }
  return x;
}
