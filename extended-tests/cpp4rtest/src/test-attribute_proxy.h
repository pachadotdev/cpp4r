#include <testthat.h>

context("attribute_proxy-C++") {
  test_that("attribute_proxy with initializer_list") {
    cpp4r::writable::doubles x({1., 2., 3.});
    x.attr("custom") = {10, 20, 30};

    cpp4r::integers attr_val(Rf_getAttrib(x.data(), Rf_install("custom")));
    expect_true(attr_val[0] == 10);
    expect_true(attr_val[1] == 20);
    expect_true(attr_val[2] == 30);
  }

  test_that("attribute_proxy with SEXP name") {
    cpp4r::writable::integers x({1, 2, 3});
    SEXP sym = Rf_install("foo");
    x.attr(sym) = "bar";

    cpp4r::strings val(Rf_getAttrib(x.data(), sym));
    expect_true(val[0] == "bar");
  }

  test_that("attribute_proxy read via SEXP conversion") {
    cpp4r::writable::doubles x({1., 2.});
    x.attr("test") = 42;

    SEXP attr_val = x.attr("test");
    expect_true(INTEGER(attr_val)[0] == 42);
  }
}
