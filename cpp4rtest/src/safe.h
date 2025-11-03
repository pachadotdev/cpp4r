/* roxygen
@title Safe functions
@rdname testing-safe
@keywords internal
*/
[[cpp4r::register]] SEXP cpp4r_safe_(SEXP x_sxp) {
  SEXP err = R_NilValue;
  const size_t ERROR_SIZE = 2048;
  char buf[ERROR_SIZE] = "";

  try {
    const cpp4r::doubles x(x_sxp);
    // Rf_error("R error"); // This will not call dtors
    // throw std::runtime_error("C++ error");
    // cpp4r::unwind_protect([&]() { Rf_error("R error"); });
    SEXP out = cpp4r::unwind_protect([&] { return Rf_allocVector(REALSXP, 1); });

    return out;
  }

  catch (cpp4r::unwind_exception& e) {
    err = e.token;
  } catch (std::exception& e) {
    strncpy(buf, e.what(), ERROR_SIZE - 1);
  }

  if (buf[0] != '\0') {
    Rf_error("%s", buf);
  } else if (err != R_NilValue) {
    R_ContinueUnwind(err);
  }

  return R_NilValue;
}
