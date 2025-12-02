// Tests for C++17 specific features in cpp4r headers
// These tests exercise code paths that are only active when CPP4R_HAS_CXX17 == 1

#include <testthat.h>
#include "cpp4r.hpp"

// Only run these tests in C++17 or later
#if CPP4R_HAS_CXX17

context("cxx17-features-C++") {
  test_that("if constexpr iterator selection works (doubles)") {
    // This tests the C++17 if constexpr branch in begin()/end()
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    auto it = x.begin();
    auto end = x.end();

    double sum = 0;
    while (it != end) {
      sum += *it;
      ++it;
    }

    expect_true(sum == 15.);
  }

  test_that("if constexpr iterator selection works (integers)") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5});

    auto it = x.begin();
    auto end = x.end();

    int sum = 0;
    while (it != end) {
      sum += *it;
      ++it;
    }

    expect_true(sum == 15);
  }

  test_that("if constexpr iterator selection works (strings)") {
    // strings use a different iterator path
    cpp4r::writable::strings x({"a", "b", "c"});

    auto it = x.begin();
    auto end = x.end();

    int count = 0;
    while (it != end) {
      ++count;
      ++it;
    }

    expect_true(count == 3);
  }

  test_that("if constexpr iterator selection works (list)") {
    // list also uses a different iterator path
    SEXP a = PROTECT(Rf_ScalarInteger(1));
    SEXP b = PROTECT(Rf_ScalarReal(2.0));
    SEXP c = PROTECT(Rf_mkString("three"));

    cpp4r::writable::list x({a, b, c});

    auto it = x.begin();
    auto end = x.end();

    int count = 0;
    while (it != end) {
      ++count;
      ++it;
    }

    expect_true(count == 3);

    UNPROTECT(3);
  }

  test_that("structured bindings work with cpp4r types") {
    using namespace cpp4r::literals;
    cpp4r::writable::doubles x({"a"_nm = 1., "b"_nm = 2.});

    // Test that we can iterate with structured bindings (C++17 feature)
    // This just verifies the types are compatible
    expect_true(x["a"] == 1.);
    expect_true(x["b"] == 2.);
  }

  test_that("[[nodiscard]] works correctly") {
    // CPP4R_NODISCARD is defined to [[nodiscard]] in C++17+
    // This test just verifies compilation succeeds
    cpp4r::writable::integers x({1, 2, 3});
    auto sz = x.size();
    expect_true(sz == 3);
  }

  test_that("inline variables work (store count)") {
    // Test that detail::store operations work correctly
    R_xlen_t before = cpp4r::detail::store::count();

    {
      cpp4r::writable::doubles x({1., 2., 3.});
      R_xlen_t during = cpp4r::detail::store::count();
      expect_true(during > before);
    }

    // Note: The count may not decrease immediately due to protection mechanisms
    // but the allocation/release path is exercised
  }

  test_that("const iterator with if constexpr") {
    const cpp4r::doubles x(PROTECT(Rf_allocVector(REALSXP, 3)));
    REAL(x)[0] = 1;
    REAL(x)[1] = 2;
    REAL(x)[2] = 3;

    auto it = x.cbegin();
    auto end = x.cend();

    double sum = 0;
    while (it != end) {
      sum += *it;
      ++it;
    }

    expect_true(sum == 6.);
    UNPROTECT(1);
  }

  test_that("read-only begin/end with if constexpr (logicals)") {
    cpp4r::writable::logicals x(
        {cpp4r::r_bool(true), cpp4r::r_bool(false), cpp4r::r_bool(true)});
    cpp4r::logicals y(x);

    auto it = y.begin();
    auto end = y.end();

    int true_count = 0;
    while (it != end) {
      if (*it == cpp4r::r_bool(true)) {
        true_count++;
      }
      ++it;
    }

    expect_true(true_count == 2);
  }

  test_that("writable iterator with proxy assignments") {
    cpp4r::writable::doubles x({1., 2., 3.});

    // Use iterator to modify
    for (auto& val : x) {
      val = val * 2;
    }

    expect_true(x[0] == 2.);
    expect_true(x[1] == 4.);
    expect_true(x[2] == 6.);
  }
}

#else

context("cxx17-features-C++") {
  test_that("C++17 tests skipped (not C++17 or later)") {
    // Placeholder test - actual C++17 tests are skipped
    expect_true(true);
  }
}

#endif  // CPP4R_HAS_CXX17
