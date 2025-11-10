#define CPP4R_USE_FMT

#include <testthat.h>

context("unwind_protect-C++") {
  test_that("unwind_protect works if there is no error") {
    SEXP out = PROTECT(cpp4r::unwind_protect([&] {
      out = PROTECT(Rf_allocVector(REALSXP, 1));
      REAL(out)[0] = 1;
      UNPROTECT(1);
      return out;
    }));

    expect_true(Rf_xlength(out) == 1);
    expect_true(REAL(out)[0] == 1);

    UNPROTECT(1);
  }
  test_that("unwind_protect throws a C++ exception if there is an R error") {
    SEXP out;
    expect_error_as(cpp4r::unwind_protect([&] {
                      out = PROTECT(Rf_allocVector(REALSXP, -1));
                      REAL(out)[0] = 1;
                      UNPROTECT(1);
                      return out;
                    }),
                    cpp4r::unwind_exception);
  }

  test_that("safe wraps R functions and works if there is no error") {
    SEXP out = PROTECT(cpp4r::safe[Rf_allocVector](REALSXP, 1));
    REAL(out)[0] = 1;

    expect_true(Rf_xlength(out) == 1);
    expect_true(REAL(out)[0] == 1);

    UNPROTECT(1);
  }

  test_that("stop throws an unwind_exception") {
    expect_error_as(cpp4r::stop("error"), cpp4r::unwind_exception);
    expect_error_as(cpp4r::stop("error {}", "message"), cpp4r::unwind_exception);
    expect_error_as(cpp4r::stop("error {a}", fmt::arg("a", "message")),
                    cpp4r::unwind_exception);
  }

  test_that("safe wraps R functions and works if there is an R error") {
    expect_error_as(cpp4r::safe[Rf_allocVector](REALSXP, -1), cpp4r::unwind_exception);
  }
}
