#include <testthat.h>

context("r_vector-advanced-C++") {
  // Test the `value()` method for writable vectors (used with variadic functions like
  // Rprintf)
  test_that("writable::doubles::value() method works") {
    cpp4r::writable::doubles x({1.0, 2.0, 3.0});
    expect_true(x.value(0) == 1.0);
    expect_true(x.value(1) == 2.0);
    expect_true(x.value(2) == 3.0);
  }

  test_that("writable::integers::value() method works") {
    cpp4r::writable::integers x({1, 2, 3});
    expect_true(x.value(0) == 1);
    expect_true(x.value(1) == 2);
    expect_true(x.value(2) == 3);
  }

  // Test cbegin/cend
  test_that("doubles::cbegin() and cend() work") {
    cpp4r::doubles x(Rf_allocVector(REALSXP, 3));
    REAL(x)[0] = 1;
    REAL(x)[1] = 2;
    REAL(x)[2] = 3;

    auto it = x.cbegin();
    expect_true(*it == 1);
    expect_true(x.cend() - x.cbegin() == 3);
  }

  // Test iterator difference
  test_that("iterator difference works") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});
    auto begin = x.begin();
    auto end = x.end();
    expect_true(end - begin == 5);

    auto mid = begin + 2;
    expect_true(mid - begin == 2);
    expect_true(end - mid == 3);
  }

  // Test const pointer access
  test_that("data_ptr() works for read-only vectors") {
    cpp4r::doubles x(Rf_allocVector(REALSXP, 3));
    REAL(x)[0] = 1;
    REAL(x)[1] = 2;
    REAL(x)[2] = 3;

    const double* ptr = x.data_ptr();
    expect_true(ptr != nullptr);
    expect_true(ptr[0] == 1);
    expect_true(ptr[1] == 2);
    expect_true(ptr[2] == 3);
  }

  test_that("data_ptr_writable() works for writable vectors") {
    cpp4r::writable::doubles x({1., 2., 3.});
    double* ptr = x.data_ptr_writable();
    expect_true(ptr != nullptr);

    ptr[0] = 10.;
    expect_true(x[0] == 10.);
  }

  // Test reserve with push_back
  test_that("reserve() and push_back() interaction") {
    cpp4r::writable::doubles x;
    x.reserve(100);
    for (int i = 0; i < 50; i++) {
      x.push_back(static_cast<double>(i));
    }
    expect_true(x.size() == 50);
  }

  // Test clear()
  test_that("clear() works") {
    cpp4r::writable::doubles x({1., 2., 3.});
    expect_true(x.size() == 3);
    x.clear();
    expect_true(x.size() == 0);
  }

  // Test truncate on SEXP conversion with capacity > length
  test_that("SEXP conversion truncates capacity") {
    cpp4r::writable::integers x(2);
    x[0] = 1;
    x[1] = 2;
    x.push_back(3);

    SEXP result = x;
    expect_true(Rf_xlength(result) == 3);
  }
}
