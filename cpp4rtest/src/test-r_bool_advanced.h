#include <testthat.h>

#include <sstream>

context("r_bool-advanced-C++") {
  test_that("r_bool from SEXP") {
    SEXP x = PROTECT(Rf_ScalarLogical(TRUE));
    cpp4r::r_bool val(x);
    expect_true(val == TRUE);
    UNPROTECT(1);
  }

  test_that("r_bool from SEXP throws on invalid input") {
    SEXP x = PROTECT(Rf_ScalarInteger(42));
    expect_error_as(cpp4r::r_bool(x), std::invalid_argument);
    UNPROTECT(1);
  }

  test_that("r_bool conversion to Rboolean") {
    cpp4r::r_bool t(true);
    Rboolean rb = t;
    expect_true(rb == TRUE);

    cpp4r::r_bool f(false);
    Rboolean rf = f;
    expect_true(rf == FALSE);
  }

  test_that("r_bool ostream operator<<") {
    std::ostringstream os;
    cpp4r::r_bool t(true);
    os << t;
    expect_true(os.str() == "TRUE");

    std::ostringstream os2;
    cpp4r::r_bool f(false);
    os2 << f;
    expect_true(os2.str() == "FALSE");
  }

  test_that("r_bool comparison with different types") {
    cpp4r::r_bool t(true);
    expect_true(t == true);
    expect_true(t == TRUE);
    expect_true(t == 1);

    cpp4r::r_bool f(false);
    expect_true(f == false);
    expect_true(f == FALSE);
  }
}
