// Tests for C++20 specific features in cpp4r headers
// These tests exercise code paths that are only active when CPP4R_HAS_CXX20 == 1

#include <testthat.h>
#include "cpp4r.hpp"

// Only run these tests in C++20 or later
#if CPP4R_HAS_CXX20

context("cxx20-features-C++") {
  test_that("[[likely]]/[[unlikely]] branch hints in operator[]") {
    // This tests the C++20 [[likely]] branch in operator[]
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    // Access elements to trigger the [[likely]] branch (data_p_ != nullptr)
    double sum = 0;
    for (R_xlen_t i = 0; i < x.size(); ++i) {
      sum += x[i];
    }

    expect_true(sum == 15.);
  }

  test_that("[[unlikely]] branch in valid_type for error paths") {
    // Test the error path that uses [[unlikely]]
    // Passing wrong type should throw
    SEXP wrong_type = PROTECT(Rf_allocVector(INTSXP, 1));

    expect_error(cpp4r::doubles(wrong_type));

    UNPROTECT(1);
  }

  test_that("[[unlikely]] in generic_const_iterator buffer refill") {
    // For ALTREP vectors, the iterator uses a buffered approach
    // This tests the [[unlikely]] branch for buffer refill
    // We need a large enough vector and strings (which use generic iterator)

    cpp4r::writable::strings x(100);
    for (R_xlen_t i = 0; i < 100; ++i) {
      x[i] = "test";
    }

    auto it = x.begin();
    auto end = x.end();

    int count = 0;
    while (it != end) {
      ++count;
      ++it;
    }

    expect_true(count == 100);
  }

  test_that("operator[] with different index types (C++20 path)") {
    cpp4r::writable::doubles x({1., 2., 3.});

    // Test with int
    int i = 0;
    expect_true(x[i] == 1.);

    // Test with R_xlen_t
    R_xlen_t r = 1;
    expect_true(x[r] == 2.);

    // Test with size_t
    size_t s = 2;
    expect_true(x[s] == 3.);
  }

  test_that("valid_type nullptr check with [[unlikely]]") {
    // The nullptr check in valid_type uses [[unlikely]]
    // This is hard to trigger directly but we can test normal path
    SEXP x = PROTECT(Rf_allocVector(REALSXP, 3));
    REAL(x)[0] = 1;
    REAL(x)[1] = 2;
    REAL(x)[2] = 3;

    cpp4r::doubles y(x);
    expect_true(y.size() == 3);

    UNPROTECT(1);
  }

  test_that("iterator increment with [[unlikely]] buffer check") {
    // Test iterator operations that might trigger buffer refill
    cpp4r::writable::list x(50);
    for (R_xlen_t i = 0; i < 50; ++i) {
      x[i] = Rf_ScalarInteger(static_cast<int>(i));
    }

    auto it = x.begin();

    // Increment through all elements
    for (R_xlen_t i = 0; i < 50; ++i) {
      ++it;
    }

    expect_true(it == x.end());
  }

  test_that("read-only vector with C++20 optimizations") {
    SEXP raw = PROTECT(Rf_allocVector(INTSXP, 1000));
    for (int i = 0; i < 1000; ++i) {
      INTEGER(raw)[i] = i;
    }

    cpp4r::integers x(raw);

    // Sum using iterator
    long long sum = 0;
    for (auto val : x) {
      sum += val;
    }

    // Sum of 0 to 999 = 999 * 1000 / 2 = 499500
    expect_true(sum == 499500LL);

    UNPROTECT(1);
  }

  test_that("writable vector move semantics with C++20") {
    cpp4r::writable::doubles x({1., 2., 3.});

    // Move construction
    cpp4r::writable::doubles y(std::move(x));

    expect_true(y.size() == 3);
    expect_true(y[0] == 1.);
    expect_true(y[1] == 2.);
    expect_true(y[2] == 3.);

    // x should be empty after move
    expect_true(x.data() == R_NilValue);
    expect_true(x.size() == 0);
  }

  test_that("complex iterators with C++20") {
    cpp4r::writable::complexes x(3);
    x[0] = cpp4r::r_complex(1.0, 2.0);
    x[1] = cpp4r::r_complex(3.0, 4.0);
    x[2] = cpp4r::r_complex(5.0, 6.0);

    auto it = x.begin();

    // Need to convert proxy to r_complex to access real()/imag()
    cpp4r::r_complex c0 = *it;
    expect_true(c0.real() == 1.0);
    expect_true(c0.imag() == 2.0);

    ++it;
    cpp4r::r_complex c1 = *it;
    expect_true(c1.real() == 3.0);
    expect_true(c1.imag() == 4.0);
  }
}

#else

context("cxx20-features-C++") {
  test_that("C++20 tests skipped (not C++20 or later)") {
    // Placeholder test - actual C++20 tests are skipped
    expect_true(true);
  }
}

#endif  // CPP4R_HAS_CXX20
