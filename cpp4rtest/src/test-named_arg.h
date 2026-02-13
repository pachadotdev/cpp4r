#include <testthat.h>

context("named_arg-C++") {
  test_that("named_arg with constructor value") {
    using namespace cpp4r::literals;

    auto arg = "x"_nm = 42;
    expect_true(strcmp(arg.name(), "x") == 0);
    expect_true(INTEGER(arg.value())[0] == 42);
  }

  test_that("named_arg with initializer_list") {
    using namespace cpp4r::literals;

    cpp4r::named_arg arg("test");
    arg = {1, 2, 3};

    expect_true(Rf_xlength(arg.value()) == 3);
    expect_true(INTEGER(arg.value())[0] == 1);
  }

  test_that("named_arg literal with assignment") {
    using namespace cpp4r::literals;

    auto arg = "foo"_nm;
    arg = 3.14;
    expect_true(REAL(arg.value())[0] == 3.14);
  }
}
