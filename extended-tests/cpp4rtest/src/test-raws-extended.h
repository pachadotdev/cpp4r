// Extended tests for raws.hpp to improve coverage
// Tests get_elt, get_p, get_const_p, get_region, set_elt, and iterators

#include <testthat.h>
#include <algorithm>
#include <numeric>

context("raws-extended-C++") {
  // ============================================
  // ALTREP simulation tests (force is_altrep=true)
  // ============================================

  test_that("raws get_p returns nullptr when is_altrep is true") {
    // Use the constructor that accepts is_altrep parameter
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, 5));
    RAW(x)[0] = 1;
    RAW(x)[1] = 2;
    RAW(x)[2] = 3;
    RAW(x)[3] = 4;
    RAW(x)[4] = 5;

    // Construct with is_altrep=true to exercise the nullptr branch in get_p
    cpp4r::raws y(x, true);

    // data_ptr() should return nullptr for ALTREP vectors
    const Rbyte* ptr = y.data_ptr();
    expect_true(ptr == nullptr);

    // But we can still access elements via get_elt
    expect_true(y[0] == 1);
    expect_true(y[1] == 2);
    expect_true(y[2] == 3);
    expect_true(y[3] == 4);
    expect_true(y[4] == 5);

    UNPROTECT(1);
  }

  test_that("raws use_buf returns true for simulated ALTREP") {
    // Create a large vector to trigger buffering behavior
    R_xlen_t n = 300;
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      RAW(x)[i] = static_cast<Rbyte>(i % 256);
    }

    // Construct with is_altrep=true
    cpp4r::raws y(x, true);

    // Iterate - with is_altrep=true, use_buf will return true
    // This exercises the buffered iteration path
    R_xlen_t count = 0;
    int sum = 0;
    for (auto it = y.begin(); it != y.end(); ++it) {
      sum += *it;
      ++count;
    }

    expect_true(count == n);
    // Sum of 0..255 + 0..43 = 32640 + 946 = 33586
    expect_true(sum == 33586);

    UNPROTECT(1);
  }

  test_that("raws iterator with simulated ALTREP exercises get_region") {
    R_xlen_t n = 400;
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      RAW(x)[i] = static_cast<Rbyte>(i % 100);
    }

    // Force ALTREP mode
    cpp4r::raws y(x, true);

    // Iterate using const iterator - triggers fill_buf and get_region
    R_xlen_t idx = 0;
    for (auto cit = y.cbegin(); cit != y.cend(); ++cit) {
      expect_true(*cit == static_cast<Rbyte>(idx % 100));
      ++idx;
    }

    expect_true(idx == n);

    UNPROTECT(1);
  }

  // ============================================
  // Read-only raws tests
  // ============================================

  test_that("raws read-only constructor from SEXP") {
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, 5));
    RAW(x)[0] = 0;
    RAW(x)[1] = 127;
    RAW(x)[2] = 128;
    RAW(x)[3] = 254;
    RAW(x)[4] = 255;

    cpp4r::raws y(x);

    expect_true(y.size() == 5);
    expect_true(y[0] == 0);
    expect_true(y[1] == 127);
    expect_true(y[2] == 128);
    expect_true(y[3] == 254);
    expect_true(y[4] == 255);

    UNPROTECT(1);
  }

  test_that("raws read-only iterator") {
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, 5));
    RAW(x)[0] = 1;
    RAW(x)[1] = 2;
    RAW(x)[2] = 3;
    RAW(x)[3] = 4;
    RAW(x)[4] = 5;

    cpp4r::raws y(x);

    auto it = y.begin();
    auto end = y.end();

    int sum = 0;
    while (it != end) {
      sum += *it;
      ++it;
    }

    expect_true(sum == 15);

    UNPROTECT(1);
  }

  test_that("raws const iterator with cbegin/cend") {
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, 3));
    RAW(x)[0] = 10;
    RAW(x)[1] = 20;
    RAW(x)[2] = 30;

    cpp4r::raws y(x);

    auto cit = y.cbegin();
    auto cend = y.cend();

    expect_true(*cit == 10);
    ++cit;
    expect_true(*cit == 20);
    ++cit;
    expect_true(*cit == 30);
    ++cit;
    expect_true(cit == cend);

    UNPROTECT(1);
  }

  test_that("raws type validation") {
    SEXP wrong_type = PROTECT(Rf_allocVector(INTSXP, 1));
    expect_error(cpp4r::raws(wrong_type));
    UNPROTECT(1);
  }

  test_that("raws empty vector") {
    cpp4r::writable::raws x;
    expect_true(x.size() == 0);
    expect_true(x.empty());
    expect_true(x.begin() == x.end());
  }

  test_that("raws data() accessor") {
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, 3));
    RAW(x)[0] = 1;
    RAW(x)[1] = 2;
    RAW(x)[2] = 3;

    cpp4r::raws y(x);

    SEXP data = y.data();
    expect_true(data != R_NilValue);
    expect_true(Rf_xlength(data) == 3);

    UNPROTECT(1);
  }

  // ============================================
  // Writable raws tests
  // ============================================

  test_that("writable raws iterator arithmetic") {
    cpp4r::writable::raws x({1, 2, 3, 4, 5});

    auto it = x.begin();

    // operator+
    auto it2 = it + 2;
    expect_true(*it2 == 3);

    // operator+=
    it += 3;
    expect_true(*it == 4);

    // Distance
    expect_true(x.end() - x.begin() == 5);
  }

  test_that("writable raws with STL algorithms") {
    cpp4r::writable::raws x({5, 3, 1, 4, 2});

    // Find
    auto it = std::find(x.begin(), x.end(), static_cast<Rbyte>(3));
    expect_true(it != x.end());
    expect_true(*it == 3);

    // Count
    cpp4r::writable::raws y({1, 2, 1, 3, 1});
    auto count = std::count(y.begin(), y.end(), static_cast<Rbyte>(1));
    expect_true(count == 3);
  }

  test_that("writable raws clear") {
    cpp4r::writable::raws x({1, 2, 3});
    expect_true(x.size() == 3);

    x.clear();
    expect_true(x.size() == 0);
    expect_true(x.empty());
  }

  test_that("writable raws reserve") {
    cpp4r::writable::raws x;
    x.reserve(100);

    // Can push_back without reallocating
    for (int i = 0; i < 50; ++i) {
      x.push_back(static_cast<Rbyte>(i));
    }

    expect_true(x.size() == 50);
  }

  test_that("writable raws copy constructor") {
    cpp4r::writable::raws x({1, 2, 3});
    cpp4r::writable::raws y(x);

    expect_true(y.size() == 3);
    expect_true(y[0] == 1);
    expect_true(y[1] == 2);
    expect_true(y[2] == 3);

    // Ensure it's a copy, not a reference
    y[0] = 99;
    expect_true(x[0] == 1);
    expect_true(y[0] == 99);
  }

  test_that("writable raws move constructor") {
    cpp4r::writable::raws x({1, 2, 3});
    cpp4r::writable::raws y(std::move(x));

    expect_true(y.size() == 3);
    expect_true(y[0] == 1);

    // x should be empty after move
    expect_true(x.data() == R_NilValue);
    expect_true(x.size() == 0);
  }

  test_that("writable raws large vector (region buffer)") {
    // Create a large vector to trigger region buffer behavior
    R_xlen_t n = 500;
    cpp4r::writable::raws x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<Rbyte>(i % 256);
    }

    // Iterate through and verify
    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      expect_true(*it == static_cast<Rbyte>(count % 256));
      ++count;
    }

    expect_true(count == n);
  }

  test_that("raws operator[] with different index types") {
    cpp4r::writable::raws x({10, 20, 30});

    int i = 0;
    R_xlen_t r = 1;
    size_t s = 2;

    expect_true(x[i] == 10);
    expect_true(x[r] == 20);
    expect_true(x[s] == 30);
  }

  test_that("raws names") {
    using namespace cpp4r::literals;

    SEXP x1 = PROTECT(Rf_allocVector(RAWSXP, 1));
    SEXP x2 = PROTECT(Rf_allocVector(RAWSXP, 1));
    RAW(x1)[0] = 1;
    RAW(x2)[0] = 2;

    cpp4r::writable::raws x({"a"_nm = x1, "b"_nm = x2});

    expect_true(x.named());
    expect_true(x["a"] == 1);
    expect_true(x["b"] == 2);

    expect_true(x.contains("a"));
    expect_true(!x.contains("c"));

    UNPROTECT(2);
  }

  test_that("raws attributes") {
    cpp4r::writable::raws x({1, 2, 3});
    x.attr("foo") = "bar";

    SEXP attr = x.attr("foo");
    expect_true(Rf_isString(attr));
  }

  test_that("read-only raws from writable") {
    cpp4r::writable::raws x({1, 2, 3});
    cpp4r::raws y(x);

    expect_true(y.size() == 3);
    expect_true(y[0] == 1);
    expect_true(y[1] == 2);
    expect_true(y[2] == 3);
  }

  // ============================================
  // Tests for get_const_p, get_region, use_buf
  // ============================================

  test_that("raws get_region with various sizes") {
    // Test with size exactly at buffer capacity boundary
    R_xlen_t n = 64;  // BUF_CAP
    cpp4r::writable::raws x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<Rbyte>(i);
    }

    // Read-only access triggers get_const_p path
    cpp4r::raws y(x);

    R_xlen_t sum = 0;
    for (R_xlen_t i = 0; i < y.size(); ++i) {
      sum += y[i];
    }

    // Sum of 0..63
    expect_true(sum == (n * (n - 1)) / 2);
  }

  test_that("raws read-only large vector iteration (triggers buffering)") {
    // Create large vector > BUF_THRESHOLD (256)
    R_xlen_t n = 300;
    SEXP x = PROTECT(Rf_allocVector(RAWSXP, n));

    for (R_xlen_t i = 0; i < n; ++i) {
      RAW(x)[i] = static_cast<Rbyte>(i % 256);
    }

    cpp4r::raws y(x);

    // Iterate using const_iterator which may use buffered reads
    R_xlen_t count = 0;
    for (auto it = y.cbegin(); it != y.cend(); ++it) {
      expect_true(*it == static_cast<Rbyte>(count % 256));
      ++count;
    }

    expect_true(count == n);

    UNPROTECT(1);
  }

  test_that("raws at() bounds checking") {
    cpp4r::writable::raws x({1, 2, 3});

    // Valid indices
    expect_true(x.at(0) == 1);
    expect_true(x.at(1) == 2);
    expect_true(x.at(2) == 3);

    // Out of bounds should throw
    expect_error(x.at(3));
    expect_error(x.at(100));
  }

  test_that("raws front and back equivalents") {
    cpp4r::writable::raws x({10, 20, 30, 40, 50});

    // Front
    expect_true(x[0] == 10);
    expect_true(*x.begin() == 10);

    // Back
    expect_true(x[x.size() - 1] == 50);
  }

  test_that("raws value method") {
    cpp4r::writable::raws x({1, 2, 3});

    // value() returns the actual value, not a proxy
    Rbyte v0 = x.value(static_cast<R_xlen_t>(0));
    Rbyte v1 = x.value(static_cast<R_xlen_t>(1));
    Rbyte v2 = x.value(static_cast<R_xlen_t>(2));

    expect_true(v0 == 1);
    expect_true(v1 == 2);
    expect_true(v2 == 3);
  }

  // ============================================
  // Direct tests for get_const_p and get_p
  // ============================================

  test_that("raws get_p returns pointer for non-ALTREP") {
    cpp4r::writable::raws x({1, 2, 3, 4, 5});

    // Access via data() and RAW
    SEXP data = x.data();
    Rbyte* p = RAW(data);

    expect_true(p != nullptr);
    expect_true(p[0] == 1);
    expect_true(p[1] == 2);
    expect_true(p[2] == 3);
    expect_true(p[3] == 4);
    expect_true(p[4] == 5);
  }

  test_that("raws read from SEXP exercises get_elt") {
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 5));
    RAW(s)[0] = 10;
    RAW(s)[1] = 20;
    RAW(s)[2] = 30;
    RAW(s)[3] = 40;
    RAW(s)[4] = 50;

    cpp4r::raws x(s);

    // Each access goes through get_elt
    expect_true(x[0] == 10);
    expect_true(x[1] == 20);
    expect_true(x[2] == 30);
    expect_true(x[3] == 40);
    expect_true(x[4] == 50);

    UNPROTECT(1);
  }

  test_that("raws set_elt via proxy") {
    cpp4r::writable::raws x({0, 0, 0});

    // Modify via proxy - this exercises set_elt
    x[0] = 100;
    x[1] = 200;
    x[2] = 255;

    expect_true(x[0] == 100);
    expect_true(x[1] == 200);
    expect_true(x[2] == 255);
  }

  test_that("raws is_altrep returns false for regular vectors") {
    cpp4r::writable::raws x({1, 2, 3});
    expect_true(!x.is_altrep());

    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 3));
    cpp4r::raws y(s);
    expect_true(!y.is_altrep());

    UNPROTECT(1);
  }

  test_that("raws RAW_OR_NULL path via get_const_p") {
    // Create read-only raws to exercise get_const_p
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 10));
    for (int i = 0; i < 10; ++i) {
      RAW(s)[i] = static_cast<Rbyte>(i * 10);
    }

    cpp4r::raws x(s);

    // Iterate - should use get_const_p internally
    int sum = 0;
    for (R_xlen_t i = 0; i < x.size(); ++i) {
      sum += x[i];
    }

    // 0 + 10 + 20 + ... + 90 = 450
    expect_true(sum == 450);

    UNPROTECT(1);
  }

  // ============================================
  // Additional coverage tests for raws.hpp
  // ============================================

  test_that("raws get_sexptype returns RAWSXP") {
    // Verify the SEXPTYPE by creating and checking
    cpp4r::writable::raws x({1, 2, 3});
    SEXP s = x.data();
    expect_true(TYPEOF(s) == RAWSXP);
  }

  test_that("raws get_elt accessed via operator[] on read-only") {
    // Create SEXP directly and wrap
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 5));
    RAW(s)[0] = 0;
    RAW(s)[1] = 127;
    RAW(s)[2] = 128;
    RAW(s)[3] = 254;
    RAW(s)[4] = 255;

    cpp4r::raws x(s);

    // Each operator[] access goes through get_elt (RAW_ELT)
    expect_true(x[0] == 0);
    expect_true(x[1] == 127);
    expect_true(x[2] == 128);
    expect_true(x[3] == 254);
    expect_true(x[4] == 255);

    UNPROTECT(1);
  }

  test_that("raws get_p for writable non-ALTREP") {
    cpp4r::writable::raws x({10, 20, 30, 40, 50});

    // Access underlying data pointer (exercises get_p path)
    Rbyte* p = x.data_ptr_writable();
    expect_true(p != nullptr);
    expect_true(p[0] == 10);
    expect_true(p[1] == 20);
    expect_true(p[2] == 30);
    expect_true(p[3] == 40);
    expect_true(p[4] == 50);
  }

  test_that("raws get_const_p for read-only non-ALTREP") {
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 4));
    RAW(s)[0] = 1;
    RAW(s)[1] = 2;
    RAW(s)[2] = 3;
    RAW(s)[3] = 4;

    cpp4r::raws x(s);

    // Access const pointer (exercises get_const_p -> RAW_OR_NULL path)
    const Rbyte* p = x.data_ptr();
    expect_true(p != nullptr);
    expect_true(p[0] == 1);
    expect_true(p[1] == 2);
    expect_true(p[2] == 3);
    expect_true(p[3] == 4);

    UNPROTECT(1);
  }

  test_that("raws set_elt via writable proxy assignment") {
    cpp4r::writable::raws x({0, 0, 0, 0, 0});

    // Each assignment exercises SET_RAW_ELT via set_elt
    x[0] = 10;
    x[1] = 50;
    x[2] = 100;
    x[3] = 200;
    x[4] = 255;

    expect_true(x[0] == 10);
    expect_true(x[1] == 50);
    expect_true(x[2] == 100);
    expect_true(x[3] == 200);
    expect_true(x[4] == 255);
  }

  test_that("raws use_buf returns true for ALTREP-like iteration") {
    // Create a larger vector to trigger buffer usage
    R_xlen_t n = 300;
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      RAW(s)[i] = static_cast<Rbyte>(i % 256);
    }

    cpp4r::raws x(s);

    // Iterate and sum - this exercises the iterator buffer logic
    int sum = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      sum += *it;
    }

    // Sum of 0..255 is 32640, plus 0..43 for remaining 44 elements
    // (300 - 256 = 44), sum of 0..43 is 946
    // Total = 32640 + 946 = 33586
    expect_true(sum == 33586);

    UNPROTECT(1);
  }

  test_that("raws get_region via iterator on large vector") {
    // Create vector > BUF_THRESHOLD to trigger get_region
    R_xlen_t n = 400;
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      RAW(s)[i] = static_cast<Rbyte>(i % 128);  // Keep values small
    }

    cpp4r::raws x(s);

    // Use const iterator which triggers buffered reads via RAW_GET_REGION
    R_xlen_t count = 0;
    int sum = 0;
    for (auto cit = x.cbegin(); cit != x.cend(); ++cit) {
      sum += *cit;
      ++count;
    }

    expect_true(count == n);
    // Sum of (0..127) * 3 + (0..15) = 3 * 8128 + 120 = 24504
    expect_true(sum == 24504);

    UNPROTECT(1);
  }

  test_that("raws initializer_list constructor") {
    cpp4r::writable::raws x({0, 127, 128, 255});

    expect_true(x.size() == 4);
    expect_true(x[0] == 0);
    expect_true(x[1] == 127);
    expect_true(x[2] == 128);
    expect_true(x[3] == 255);
  }

  test_that("raws push_back exercises set_elt") {
    cpp4r::writable::raws x;
    x.reserve(5);

    x.push_back(static_cast<Rbyte>(10));
    x.push_back(static_cast<Rbyte>(20));
    x.push_back(static_cast<Rbyte>(30));
    x.push_back(static_cast<Rbyte>(40));
    x.push_back(static_cast<Rbyte>(50));

    expect_true(x.size() == 5);
    expect_true(x[0] == 10);
    expect_true(x[1] == 20);
    expect_true(x[2] == 30);
    expect_true(x[3] == 40);
    expect_true(x[4] == 50);
  }

  test_that("raws data_ptr returns RAW pointer") {
    cpp4r::writable::raws x({1, 2, 3});

    // data_ptr_writable returns the underlying Rbyte* for raws
    Rbyte* ptr = x.data_ptr_writable();

    if (ptr != nullptr) {
      // Modify via pointer
      ptr[0] = 100;
      ptr[1] = 200;

      // Verify changes
      expect_true(x[0] == 100);
      expect_true(x[1] == 200);
    }
  }

  test_that("raws iteration exercises all iterator paths") {
    // Small vector (no buffering)
    cpp4r::writable::raws small({1, 2, 3});
    R_xlen_t small_count = 0;
    for (auto it = small.begin(); it != small.end(); ++it) {
      ++small_count;
    }
    expect_true(small_count == 3);

    // Medium vector (at threshold)
    R_xlen_t med_n = 256;
    cpp4r::writable::raws medium(med_n);
    for (R_xlen_t i = 0; i < med_n; ++i) {
      medium[i] = static_cast<Rbyte>(i % 256);
    }
    R_xlen_t med_count = 0;
    for (auto it = medium.begin(); it != medium.end(); ++it) {
      ++med_count;
    }
    expect_true(med_count == med_n);

    // Large vector (buffering active)
    R_xlen_t large_n = 512;
    cpp4r::writable::raws large(large_n);
    for (R_xlen_t i = 0; i < large_n; ++i) {
      large[i] = static_cast<Rbyte>(i % 256);
    }
    R_xlen_t large_count = 0;
    for (auto it = large.begin(); it != large.end(); ++it) {
      ++large_count;
    }
    expect_true(large_count == large_n);
  }

  test_that("raws direct RAW_ELT access") {
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 3));
    RAW(s)[0] = 11;
    RAW(s)[1] = 22;
    RAW(s)[2] = 33;

    cpp4r::raws x(s);

    // Verify get_elt is correctly reading RAW_ELT values
    expect_true(x[0] == 11);
    expect_true(x[1] == 22);
    expect_true(x[2] == 33);

    UNPROTECT(1);
  }

  test_that("raws set_elt with different values") {
    cpp4r::writable::raws x(10);

    // Set various byte values to fully exercise set_elt
    x[0] = 0;
    x[1] = 1;
    x[2] = 127;
    x[3] = 128;
    x[4] = 200;
    x[5] = 254;
    x[6] = 255;
    x[7] = 100;
    x[8] = 50;
    x[9] = 25;

    expect_true(x[0] == 0);
    expect_true(x[1] == 1);
    expect_true(x[2] == 127);
    expect_true(x[3] == 128);
    expect_true(x[4] == 200);
    expect_true(x[5] == 254);
    expect_true(x[6] == 255);
    expect_true(x[7] == 100);
    expect_true(x[8] == 50);
    expect_true(x[9] == 25);
  }

  test_that("raws proxy read and write cycle") {
    cpp4r::writable::raws x({10, 20, 30});

    // Read via proxy, modify, verify
    Rbyte v0 = x[0];
    Rbyte v1 = x[1];
    Rbyte v2 = x[2];

    expect_true(v0 == 10);
    expect_true(v1 == 20);
    expect_true(v2 == 30);

    // Write via proxy
    x[0] = v1;  // x[0] = 20
    x[1] = v2;  // x[1] = 30
    x[2] = v0;  // x[2] = 10

    expect_true(x[0] == 20);
    expect_true(x[1] == 30);
    expect_true(x[2] == 10);
  }

  test_that("raws read-only const_iterator dereferencing") {
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, 5));
    for (int i = 0; i < 5; ++i) {
      RAW(s)[i] = static_cast<Rbyte>(i * 25);
    }

    cpp4r::raws x(s);

    auto it = x.begin();
    expect_true(*it == 0);
    ++it;
    expect_true(*it == 25);
    ++it;
    expect_true(*it == 50);
    ++it;
    expect_true(*it == 75);
    ++it;
    expect_true(*it == 100);

    UNPROTECT(1);
  }

  test_that("raws boundary values") {
    cpp4r::writable::raws x({0, 1, 126, 127, 128, 129, 254, 255});

    expect_true(x[0] == 0);  // min value
    expect_true(x[1] == 1);
    expect_true(x[2] == 126);
    expect_true(x[3] == 127);  // max signed byte if treated as int8
    expect_true(x[4] == 128);  // min negative if treated as int8
    expect_true(x[5] == 129);
    expect_true(x[6] == 254);
    expect_true(x[7] == 255);  // max value
  }
}
