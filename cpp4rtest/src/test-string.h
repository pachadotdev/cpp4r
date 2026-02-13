#include <testthat.h>

context("string-C++") {
  test_that("is_na(string)") {
    cpp4r::r_string x("foo");
    expect_true(!cpp4r::is_na(x));

    cpp4r::r_string y(NA_STRING);
    expect_true(cpp4r::is_na(y));
  }
}
