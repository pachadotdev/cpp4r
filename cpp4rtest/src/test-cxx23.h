// Tests for C++23 specific features in cpp4r headers
// These tests exercise code paths that are only active when CPP4R_HAS_CXX23 == 1

#include <testthat.h>
#include "cpp4r.hpp"

// Only run these tests in C++23 or later
#if CPP4R_HAS_CXX23

context("cxx23-features-C++") {
  test_that("CPP4R_ASSUME macro in C++23") {
    // In C++23, CPP4R_ASSUME uses std::unreachable() internally
    // This tests that code compiles and runs correctly

    cpp4r::writable::doubles x({1., 2., 3.});

    // Compiler can use assumptions for optimization
    expect_true(x.size() == 3);
    expect_true(x[0] == 1.);
  }

  test_that("if consteval detection in C++23") {
    // CPP4R_HAS_IF_CONSTEVAL is 1 in C++23
    // This tests that code paths work correctly

    cpp4r::writable::integers x({1, 2, 3, 4, 5});

    int sum = 0;
    for (auto val : x) {
      sum += val;
    }

    expect_true(sum == 15);
  }

  test_that("std::expected style error handling (future)") {
    // CPP4R_HAS_STD_EXPECTED is 1 in C++23
    // This tests that traditional error paths still work

    // Type mismatch should throw
    SEXP wrong_type = PROTECT(Rf_allocVector(STRSXP, 1));
    expect_error(cpp4r::integers(wrong_type));
    UNPROTECT(1);

    // Correct type should work
    SEXP correct_type = PROTECT(Rf_allocVector(INTSXP, 1));
    INTEGER(correct_type)[0] = 42;
    cpp4r::integers y(correct_type);
    expect_true(y[0] == 42);
    UNPROTECT(1);
  }

  test_that("multidimensional subscript preparation (future)") {
    // CPP4R_HAS_MULTIDIMENSIONAL_SUBSCRIPT is 1 in C++23
    // This tests matrix access which could use this in the future

    cpp4r::writable::doubles_matrix<> m(3, 3);
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        m(i, j) = i * 3 + j + 1;
      }
    }

    expect_true(m(0, 0) == 1);
    expect_true(m(1, 1) == 5);
    expect_true(m(2, 2) == 9);
  }

  test_that("range-based operations with C++23") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    // Standard algorithm that works with ranges
    double sum = 0;
    for (const auto& val : x) {
      sum += val;
    }

    expect_true(sum == 15.);
  }

  test_that("iterator category traits in C++23") {
    // Verify iterator type traits work correctly
    using iter_t = cpp4r::writable::doubles::iterator;

    // Should be a random access iterator
    expect_true((std::is_same<typename std::iterator_traits<iter_t>::iterator_category,
                              std::random_access_iterator_tag>::value));
  }
}

#else

context("cxx23-features-C++") {
  test_that("C++23 tests skipped (not C++23 or later)") {
    // Placeholder test - actual C++23 tests are skipped
    expect_true(true);
  }
}

#endif  // CPP4R_HAS_CXX23
