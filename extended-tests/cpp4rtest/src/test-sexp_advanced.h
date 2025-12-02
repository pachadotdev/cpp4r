#include <testthat.h>

context("sexp-advanced-C++") {
  test_that("sexp copy assignment operator") {
    cpp4r::sexp x(Rf_ScalarReal(1.0));
    cpp4r::sexp y(Rf_ScalarReal(2.0));

    y = x;
    expect_true(REAL(y)[0] == 1.0);
  }

  test_that("sexp attr(SEXP) works") {
    cpp4r::sexp x(Rf_allocVector(REALSXP, 1));
    x.attr(R_ClassSymbol) = "myclass";

    SEXP cls = Rf_getAttrib(x, R_ClassSymbol);
    expect_true(strcmp(CHAR(STRING_ELT(cls, 0)), "myclass") == 0);
  }

  test_that("sexp attr(std::string) works") {
    cpp4r::sexp x(Rf_allocVector(REALSXP, 1));
    std::string attr_name = "foo";
    x.attr(attr_name) = "bar";

    SEXP attr_val = Rf_getAttrib(x, Rf_install("foo"));
    expect_true(strcmp(CHAR(STRING_ELT(attr_val, 0)), "bar") == 0);
  }

  test_that("sexp names() works") {
    using namespace cpp4r::literals;
    cpp4r::writable::doubles x({"a"_nm = 1., "b"_nm = 2.});
    cpp4r::sexp s(x);  // Works in C++14 and C++17+ thanks to templated constructor

    SEXP names = s.names();  // attribute_proxy converts to SEXP
    expect_true(Rf_xlength(names) == 2);
  }

  test_that("sexp deprecated conversion operators") {
    // These are deprecated but should still work
    cpp4r::sexp d(Rf_ScalarReal(5.5));
    double dval = d;  // deprecated
    expect_true(dval == 5.5);

    cpp4r::sexp l(Rf_ScalarLogical(TRUE));
    bool bval = l;  // deprecated
    expect_true(bval == true);
  }
}
