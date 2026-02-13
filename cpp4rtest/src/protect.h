/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_one_(SEXP x, int n) {
  for (R_xlen_t i = 0; i < n; ++i) {
    PROTECT(x);
    UNPROTECT(1);
  }
}

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_one_sexp_(SEXP x, int n) {
  for (R_xlen_t i = 0; i < n; ++i) {
    cpp4r::sexp y(x);
  }
}

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_one_cpp4r_(SEXP x, int n) {
  for (R_xlen_t i = 0; i < n; ++i) {
    SEXP p = cpp4r::detail::store::insert(x);
    cpp4r::detail::store::release(p);
  }
}

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_one_preserve_(SEXP x, int n) {
  for (R_xlen_t i = 0; i < n; ++i) {
    R_PreserveObject(x);
    R_ReleaseObject(x);
  }
}

// The internal protections here are actually uneeded, but it is a useful way to benchmark
// them

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_many_(int n) {
  std::vector<SEXP> res;
  for (R_xlen_t i = 0; i < n; ++i) {
    res.push_back(PROTECT(Rf_ScalarInteger(n)));
  }

  for (R_xlen_t i = n - 1; i >= 0; --i) {
    SEXP x = res[i];
    (void)x;  // silence unused variable warning while preserving behavior
    UNPROTECT(1);
    res.pop_back();
  }
}

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_many_cpp4r_(int n) {
  std::vector<SEXP> res;
  for (R_xlen_t i = 0; i < n; ++i) {
    res.push_back(cpp4r::detail::store::insert(Rf_ScalarInteger(n)));
  }

  for (R_xlen_t i = n - 1; i >= 0; --i) {
    SEXP x = res[i];
    cpp4r::detail::store::release(x);
    res.pop_back();
  }
}

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_many_sexp_(int n) {
  std::vector<cpp4r::sexp> res;
  for (R_xlen_t i = 0; i < n; ++i) {
    res.push_back(Rf_ScalarInteger(n));
  }

  for (R_xlen_t i = n - 1; i >= 0; --i) {
    res.pop_back();
  }
}

/* roxygen
@title Protect functions
@rdname testing-protect
@keywords internal
*/
[[cpp4r::register]] void protect_many_preserve_(int n) {
  std::vector<cpp4r::sexp> res;
  for (R_xlen_t i = 0; i < n; ++i) {
    SEXP x = Rf_ScalarInteger(n);
    R_PreserveObject(x);
    res.push_back(x);
  }

  for (R_xlen_t i = n - 1; i >= 0; --i) {
    SEXP x = res[i];
    R_ReleaseObject(x);
    res.pop_back();
  }
}
