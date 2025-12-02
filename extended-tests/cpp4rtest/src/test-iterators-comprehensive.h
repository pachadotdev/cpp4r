// Comprehensive iterator tests for coverage
// Tests all iterator operations across different vector types

#include <testthat.h>
#include <algorithm>
#include <numeric>
#include "cpp4r.hpp"

context("iterators-comprehensive-C++") {
  // ============================================
  // Random Access Iterator Requirements
  // ============================================

  test_that("iterator arithmetic (doubles)") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    auto it = x.begin();

    // operator+
    auto it2 = it + 2;
    expect_true(*it2 == 3.);

    // operator-
    auto it3 = it2 - 1;
    expect_true(*it3 == 2.);

    // difference
    expect_true(it2 - it == 2);
    expect_true(it3 - it == 1);
  }

  test_that("iterator comparison operators") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5});

    auto it1 = x.begin();
    auto it2 = x.begin() + 2;
    auto it3 = x.end();

    // Less than
    expect_true(it1 < it2);
    expect_true(it2 < it3);
    expect_true(!(it2 < it1));

    // Greater than
    expect_true(it2 > it1);
    expect_true(it3 > it2);

    // Less than or equal
    expect_true(it1 <= it2);
    expect_true(it1 <= it1);

    // Greater than or equal
    expect_true(it2 >= it1);
    expect_true(it2 >= it2);
  }

  test_that("iterator subscript operator") {
    cpp4r::writable::doubles x({10., 20., 30., 40., 50.});

    auto it = x.begin();

    expect_true(it[0] == 10.);
    expect_true(it[1] == 20.);
    expect_true(it[2] == 30.);
    expect_true(it[4] == 50.);
  }

  test_that("post-increment and post-decrement") {
    cpp4r::writable::integers x({1, 2, 3});

    auto it = x.begin();

    // Post-increment returns old value
    auto old = it++;
    expect_true(*old == 1);
    expect_true(*it == 2);

    // Post-decrement returns old value
    old = it--;
    expect_true(*old == 2);
    expect_true(*it == 1);
  }

  test_that("pre-increment and pre-decrement") {
    cpp4r::writable::integers x({1, 2, 3});

    auto it = x.begin();

    // Pre-increment returns new value
    expect_true(*(++it) == 2);
    expect_true(*it == 2);

    // Pre-decrement returns new value
    expect_true(*(--it) == 1);
    expect_true(*it == 1);
  }

  test_that("operator+= and operator-=") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    auto it = x.begin();

    it += 3;
    expect_true(*it == 4.);

    it -= 2;
    expect_true(*it == 2.);
  }

  // ============================================
  // Read-only Iterator Tests
  // ============================================

  test_that("const_iterator for doubles") {
    SEXP raw = PROTECT(Rf_allocVector(REALSXP, 5));
    REAL(raw)[0] = 1;
    REAL(raw)[1] = 2;
    REAL(raw)[2] = 3;
    REAL(raw)[3] = 4;
    REAL(raw)[4] = 5;

    const cpp4r::doubles x(raw);

    auto it = x.begin();
    auto end = x.end();

    double sum = 0;
    while (it != end) {
      sum += *it;
      ++it;
    }

    expect_true(sum == 15.);
    UNPROTECT(1);
  }

  test_that("const_iterator for integers") {
    SEXP raw = PROTECT(Rf_allocVector(INTSXP, 5));
    INTEGER(raw)[0] = 1;
    INTEGER(raw)[1] = 2;
    INTEGER(raw)[2] = 3;
    INTEGER(raw)[3] = 4;
    INTEGER(raw)[4] = 5;

    cpp4r::integers x(raw);

    int sum = 0;
    for (auto val : x) {
      sum += val;
    }

    expect_true(sum == 15);
    UNPROTECT(1);
  }

  test_that("const_iterator for logicals") {
    cpp4r::writable::logicals temp({cpp4r::r_bool(true), cpp4r::r_bool(false),
                                    cpp4r::r_bool(true), cpp4r::r_bool(true)});
    cpp4r::logicals x(temp);

    int true_count = 0;
    for (auto val : x) {
      if (val == cpp4r::r_bool(true)) {
        true_count++;
      }
    }

    expect_true(true_count == 3);
  }

  // ============================================
  // Generic Iterator Tests (strings, list)
  // ============================================

  test_that("generic iterator for strings") {
    cpp4r::writable::strings x({"hello", "world", "test"});

    auto it = x.begin();

    expect_true(*it == "hello");
    ++it;
    expect_true(*it == "world");
    ++it;
    expect_true(*it == "test");
  }

  test_that("generic iterator for list") {
    SEXP a = PROTECT(Rf_ScalarInteger(1));
    SEXP b = PROTECT(Rf_ScalarReal(2.5));
    SEXP c = PROTECT(Rf_mkString("three"));

    cpp4r::writable::list x({a, b, c});

    auto it = x.begin();

    // First element is integer
    SEXP elem1 = *it;
    expect_true(Rf_isInteger(elem1));
    expect_true(INTEGER(elem1)[0] == 1);

    ++it;

    // Second element is real
    SEXP elem2 = *it;
    expect_true(Rf_isReal(elem2));
    expect_true(REAL(elem2)[0] == 2.5);

    ++it;

    // Third element is string
    SEXP elem3 = *it;
    expect_true(Rf_isString(elem3));

    UNPROTECT(3);
  }

  test_that("generic iterator with reverse iteration") {
    cpp4r::writable::strings x({"a", "b", "c", "d", "e"});

    auto it = x.end();
    --it;

    expect_true(*it == "e");
    --it;
    expect_true(*it == "d");
    --it;
    expect_true(*it == "c");
  }

  // ============================================
  // STL Algorithm Compatibility
  // ============================================

  test_that("std::find works with iterators") {
    cpp4r::writable::integers x({10, 20, 30, 40, 50});

    auto it = std::find(x.begin(), x.end(), 30);
    expect_true(it != x.end());
    expect_true(*it == 30);

    auto not_found = std::find(x.begin(), x.end(), 99);
    expect_true(not_found == x.end());
  }

  test_that("std::count works with iterators") {
    cpp4r::writable::integers x({1, 2, 2, 3, 2, 4, 2});

    auto count = std::count(x.begin(), x.end(), 2);
    expect_true(count == 4);
  }

  test_that("std::accumulate works with iterators") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    double sum = std::accumulate(x.begin(), x.end(), 0.);
    expect_true(sum == 15.);
  }

  test_that("std::min_element and std::max_element") {
    cpp4r::writable::integers x({5, 2, 8, 1, 9, 3});

    auto min_it = std::min_element(x.begin(), x.end());
    expect_true(*min_it == 1);

    auto max_it = std::max_element(x.begin(), x.end());
    expect_true(*max_it == 9);
  }

  test_that("std::distance works with iterators") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});

    auto dist = std::distance(x.begin(), x.end());
    expect_true(dist == 5);

    auto it = x.begin() + 2;
    expect_true(std::distance(x.begin(), it) == 2);
  }

  test_that("std::transform works with writable iterators") {
    cpp4r::writable::doubles x({1., 2., 3., 4., 5.});
    cpp4r::writable::doubles y(5);

    std::transform(x.begin(), x.end(), y.begin(), [](double v) { return v * 2; });

    expect_true(y[0] == 2.);
    expect_true(y[1] == 4.);
    expect_true(y[2] == 6.);
    expect_true(y[3] == 8.);
    expect_true(y[4] == 10.);
  }

  test_that("std::copy works with iterators") {
    cpp4r::writable::integers src({1, 2, 3, 4, 5});
    cpp4r::writable::integers dst(5);

    std::copy(src.begin(), src.end(), dst.begin());

    for (R_xlen_t i = 0; i < 5; ++i) {
      expect_true(dst[i] == src[i]);
    }
  }

  // ============================================
  // Large Vector Tests (Buffer Behavior)
  // ============================================

  test_that("iterator with large vector (strings)") {
    // Large enough to trigger buffer behavior in generic iterator
    R_xlen_t n = 1000;
    cpp4r::writable::strings x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = "test";
    }

    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == n);
  }

  test_that("iterator with large vector (list)") {
    R_xlen_t n = 500;
    cpp4r::writable::list x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = Rf_ScalarInteger(static_cast<int>(i));
    }

    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == n);
  }

  // ============================================
  // Edge Cases
  // ============================================

  test_that("iterator on empty vector") {
    cpp4r::writable::doubles x;

    expect_true(x.begin() == x.end());
    expect_true(x.size() == 0);
  }

  test_that("iterator on single element vector") {
    cpp4r::writable::integers x({42});

    auto it = x.begin();
    expect_true(*it == 42);
    ++it;
    expect_true(it == x.end());
  }

  test_that("cbegin and cend") {
    cpp4r::writable::doubles x({1., 2., 3.});

    auto cit = x.cbegin();
    auto cend = x.cend();

    double sum = 0;
    while (cit != cend) {
      sum += *cit;
      ++cit;
    }

    expect_true(sum == 6.);
  }
}
