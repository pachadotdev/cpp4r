#include <testthat.h>

context("nas-C++") {
  test_that("na integer") { expect_true(cpp4r::na<int>() == NA_INTEGER); }
  test_that("na double") { expect_true(ISNA(cpp4r::na<double>())); }
  test_that("na bool") { expect_true(cpp4r::na<cpp4r::r_bool>() == NA_LOGICAL); }
  test_that("na string") { expect_true(cpp4r::na<cpp4r::r_string>() == NA_STRING); }
}
