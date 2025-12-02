// Extended tests for logicals.hpp to improve coverage
// Tests get_elt, get_p, get_const_p, get_region, set_elt, proxy operator==, and iterators

#include <testthat.h>
#include <algorithm>

context("logicals-extended-C++") {
  // ============================================
  // ALTREP simulation tests (force is_altrep=true)
  // ============================================

  test_that("logicals use_buf returns true for simulated ALTREP") {
    // Create a large vector to trigger buffering behavior
    R_xlen_t n = 300;
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      LOGICAL(x)[i] = (i % 2 == 0) ? TRUE : FALSE;
    }

    // Construct with is_altrep=true to exercise use_buf returning true
    cpp4r::logicals y(x, true);

    // Iterate - with is_altrep=true, use_buf will return true
    // This exercises the buffered iteration path
    R_xlen_t count = 0;
    R_xlen_t true_count = 0;
    for (auto it = y.begin(); it != y.end(); ++it) {
      if (*it == TRUE) ++true_count;
      ++count;
    }

    expect_true(count == n);
    expect_true(true_count == 150);  // Every other element

    UNPROTECT(1);
  }

  test_that("logicals iterator with simulated ALTREP exercises get_region") {
    R_xlen_t n = 400;
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      LOGICAL(x)[i] = (i % 3 == 0) ? TRUE : FALSE;
    }

    // Force ALTREP mode
    cpp4r::logicals y(x, true);

    // Iterate using const iterator - triggers fill_buf and get_region
    R_xlen_t idx = 0;
    R_xlen_t true_count = 0;
    for (auto cit = y.cbegin(); cit != y.cend(); ++cit) {
      if (*cit == TRUE) ++true_count;
      ++idx;
    }

    expect_true(idx == n);
    // 0, 3, 6, ..., 399 -> 134 values
    expect_true(true_count == 134);

    UNPROTECT(1);
  }

  test_that("logicals simulated ALTREP still allows element access via get_elt") {
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, 5));
    LOGICAL(x)[0] = TRUE;
    LOGICAL(x)[1] = FALSE;
    LOGICAL(x)[2] = NA_LOGICAL;
    LOGICAL(x)[3] = TRUE;
    LOGICAL(x)[4] = FALSE;

    // Construct with is_altrep=true
    cpp4r::logicals y(x, true);

    // Access elements - should use get_elt path since data_p_ will not be used
    expect_true(y[0] == TRUE);
    expect_true(y[1] == FALSE);
    expect_true(y[2] == NA_LOGICAL);
    expect_true(y[3] == TRUE);
    expect_true(y[4] == FALSE);

    UNPROTECT(1);
  }

  // ============================================
  // Read-only logicals tests
  // ============================================

  test_that("logicals read-only constructor from SEXP") {
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, 4));
    LOGICAL(x)[0] = TRUE;
    LOGICAL(x)[1] = FALSE;
    LOGICAL(x)[2] = NA_LOGICAL;
    LOGICAL(x)[3] = TRUE;

    cpp4r::logicals y(x);

    expect_true(y.size() == 4);
    expect_true(y[0] == TRUE);
    expect_true(y[1] == FALSE);
    expect_true(y[2] == NA_LOGICAL);
    expect_true(y[3] == TRUE);

    UNPROTECT(1);
  }

  test_that("logicals read-only iterator") {
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, 3));
    LOGICAL(x)[0] = TRUE;
    LOGICAL(x)[1] = FALSE;
    LOGICAL(x)[2] = TRUE;

    cpp4r::logicals y(x);

    auto it = y.begin();
    auto end = y.end();

    int count_true = 0;
    while (it != end) {
      if (*it == TRUE) ++count_true;
      ++it;
    }

    expect_true(count_true == 2);

    UNPROTECT(1);
  }

  test_that("logicals const iterator with cbegin/cend") {
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, 3));
    LOGICAL(x)[0] = TRUE;
    LOGICAL(x)[1] = FALSE;
    LOGICAL(x)[2] = TRUE;

    cpp4r::logicals y(x);

    auto cit = y.cbegin();
    auto cend = y.cend();

    expect_true(*cit == TRUE);
    ++cit;
    expect_true(*cit == FALSE);
    ++cit;
    expect_true(*cit == TRUE);
    ++cit;
    expect_true(cit == cend);

    UNPROTECT(1);
  }

  test_that("logicals type validation") {
    SEXP wrong_type = PROTECT(Rf_allocVector(INTSXP, 1));
    expect_error(cpp4r::logicals(wrong_type));
    UNPROTECT(1);
  }

  test_that("logicals empty vector") {
    cpp4r::writable::logicals x;
    expect_true(x.size() == 0);
    expect_true(x.empty());
    expect_true(x.begin() == x.end());
  }

  test_that("logicals data() accessor") {
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, 3));
    LOGICAL(x)[0] = TRUE;
    LOGICAL(x)[1] = FALSE;
    LOGICAL(x)[2] = TRUE;

    cpp4r::logicals y(x);

    SEXP data = y.data();
    expect_true(data != R_NilValue);
    expect_true(Rf_xlength(data) == 3);

    UNPROTECT(1);
  }

  // ============================================
  // r_bool proxy tests
  // ============================================

  test_that("r_bool proxy operator== with various types") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // r_bool == int
    expect_true(static_cast<cpp4r::r_bool>(x[0]) == 1);
    expect_true(static_cast<cpp4r::r_bool>(x[1]) == 0);
    expect_true(!(static_cast<cpp4r::r_bool>(x[2]) == 0));
    expect_true(!(static_cast<cpp4r::r_bool>(x[2]) == 1));

    // r_bool == NA_LOGICAL
    expect_true(static_cast<cpp4r::r_bool>(x[2]) == NA_LOGICAL);
  }

  test_that("r_bool implicit conversions") {
    cpp4r::writable::logicals x({TRUE, FALSE});

    // Conversion to bool (need to go through r_bool first)
    cpp4r::r_bool rb1 = x[0];
    cpp4r::r_bool rb2 = x[1];
    bool b1 = static_cast<bool>(rb1);
    bool b2 = static_cast<bool>(rb2);

    expect_true(b1 == true);
    expect_true(b2 == false);

    // Conversion to int
    int i1 = static_cast<int>(static_cast<cpp4r::r_bool>(x[0]));
    int i2 = static_cast<int>(static_cast<cpp4r::r_bool>(x[1]));

    expect_true(i1 == 1);
    expect_true(i2 == 0);
  }

  test_that("r_bool NA handling") {
    cpp4r::writable::logicals x{NA_LOGICAL};

    cpp4r::r_bool rb = x[0];
    expect_true(rb == NA_LOGICAL);

    // NA should compare equal to NA_LOGICAL
    expect_true(static_cast<int>(rb) == NA_LOGICAL);
  }

  // ============================================
  // Writable logicals tests
  // ============================================

  test_that("writable logicals iterator arithmetic") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE, FALSE, TRUE});

    auto it = x.begin();

    // operator+
    auto it2 = it + 2;
    expect_true(*it2 == TRUE);

    // operator+=
    it += 3;
    expect_true(*it == FALSE);

    // Distance
    expect_true(x.end() - x.begin() == 5);
  }

  test_that("writable logicals with STL algorithms") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE, TRUE, FALSE});

    // Count
    auto count_true = std::count_if(x.begin(), x.end(),
                                    [](cpp4r::r_bool b) { return static_cast<bool>(b); });
    expect_true(count_true == 3);

    auto count_false = std::count_if(
        x.begin(), x.end(), [](cpp4r::r_bool b) { return !static_cast<bool>(b); });
    expect_true(count_false == 2);
  }

  test_that("writable logicals clear") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});
    expect_true(x.size() == 3);

    x.clear();
    expect_true(x.size() == 0);
    expect_true(x.empty());
  }

  test_that("writable logicals reserve") {
    cpp4r::writable::logicals x;
    x.reserve(100);

    // Can push_back without reallocating
    for (int i = 0; i < 50; ++i) {
      x.push_back(i % 2 == 0);
    }

    expect_true(x.size() == 50);
  }

  test_that("writable logicals copy constructor") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});
    cpp4r::writable::logicals y(x);

    expect_true(y.size() == 3);
    expect_true(y[0] == TRUE);
    expect_true(y[1] == FALSE);
    expect_true(y[2] == TRUE);

    // Ensure it's a copy, not a reference
    y[0] = FALSE;
    expect_true(x[0] == TRUE);
    expect_true(y[0] == FALSE);
  }

  test_that("writable logicals move constructor") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});
    cpp4r::writable::logicals y(std::move(x));

    expect_true(y.size() == 3);
    expect_true(y[0] == TRUE);

    // x should be empty after move
    expect_true(x.data() == R_NilValue);
    expect_true(x.size() == 0);
  }

  test_that("writable logicals large vector (region buffer)") {
    // Create a large vector to trigger region buffer behavior
    R_xlen_t n = 500;
    cpp4r::writable::logicals x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = (i % 2 == 0);
    }

    // Iterate through and verify
    R_xlen_t count = 0;
    int count_true = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      if (*it == TRUE) ++count_true;
      ++count;
    }

    expect_true(count == n);
    expect_true(count_true == 250);
  }

  test_that("logicals operator[] with different index types") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});

    int i = 0;
    R_xlen_t r = 1;
    size_t s = 2;

    expect_true(x[i] == TRUE);
    expect_true(x[r] == FALSE);
    expect_true(x[s] == TRUE);
  }

  test_that("logicals names") {
    using namespace cpp4r::literals;

    cpp4r::writable::logicals x({"a"_nm = cpp4r::r_bool(TRUE),
                                 "b"_nm = cpp4r::r_bool(FALSE),
                                 "c"_nm = cpp4r::r_bool(NA_LOGICAL)});

    expect_true(x.named());
    expect_true(x["a"] == TRUE);
    expect_true(x["b"] == FALSE);
    expect_true(x["c"] == NA_LOGICAL);

    expect_true(x.contains("a"));
    expect_true(!x.contains("d"));
  }

  test_that("logicals attributes") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});
    x.attr("foo") = "bar";

    SEXP attr = x.attr("foo");
    expect_true(Rf_isString(attr));
  }

  test_that("read-only logicals from writable") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});
    cpp4r::logicals y(x);

    expect_true(y.size() == 3);
    expect_true(y[0] == TRUE);
    expect_true(y[1] == FALSE);
    expect_true(y[2] == TRUE);
  }

  // ============================================
  // Logical operations
  // ============================================

  test_that("logicals all_true / any_true like patterns") {
    cpp4r::writable::logicals x({TRUE, TRUE, TRUE});

    bool all_true = true;
    for (auto it = x.begin(); it != x.end(); ++it) {
      if (*it == FALSE) all_true = false;
    }
    expect_true(all_true);

    cpp4r::writable::logicals y({TRUE, FALSE, TRUE});
    all_true = true;
    for (auto it = y.begin(); it != y.end(); ++it) {
      if (*it == FALSE) all_true = false;
    }
    expect_true(!all_true);
  }

  test_that("logicals toggle values") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});

    for (R_xlen_t i = 0; i < x.size(); ++i) {
      if (x[i] == TRUE) {
        x[i] = FALSE;
      } else {
        x[i] = TRUE;
      }
    }

    expect_true(x[0] == FALSE);
    expect_true(x[1] == TRUE);
    expect_true(x[2] == FALSE);
  }

  // ============================================
  // Tests for get_const_p, get_region, use_buf
  // ============================================

  test_that("logicals get_region with various sizes") {
    // Test with size exactly at buffer capacity boundary
    R_xlen_t n = 64;  // BUF_CAP
    cpp4r::writable::logicals x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = (i % 2 == 0);
    }

    // Read-only access triggers get_const_p path
    cpp4r::logicals y(x);

    R_xlen_t count_true = 0;
    for (R_xlen_t i = 0; i < y.size(); ++i) {
      if (y[i] == TRUE) ++count_true;
    }

    expect_true(count_true == 32);
  }

  test_that("logicals read-only large vector iteration (triggers buffering)") {
    // Create large vector > BUF_THRESHOLD (256)
    R_xlen_t n = 300;
    SEXP x = PROTECT(Rf_allocVector(LGLSXP, n));

    for (R_xlen_t i = 0; i < n; ++i) {
      LOGICAL(x)[i] = (i % 3 == 0) ? TRUE : FALSE;
    }

    cpp4r::logicals y(x);

    // Iterate using const_iterator which may use buffered reads
    R_xlen_t count = 0;
    R_xlen_t count_true = 0;
    for (auto it = y.cbegin(); it != y.cend(); ++it) {
      if (*it == TRUE) ++count_true;
      ++count;
    }

    expect_true(count == n);
    expect_true(count_true == 100);  // 0, 3, 6, ... 297 = 100 values

    UNPROTECT(1);
  }

  test_that("logicals at() bounds checking") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});

    // Valid indices
    expect_true(x.at(0) == TRUE);
    expect_true(x.at(1) == FALSE);
    expect_true(x.at(2) == TRUE);

    // Out of bounds should throw
    expect_error(x.at(3));
    expect_error(x.at(100));
  }

  test_that("logicals front and back equivalents") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE, FALSE, TRUE});

    // Front
    expect_true(x[0] == TRUE);
    expect_true(*x.begin() == TRUE);

    // Back
    expect_true(x[x.size() - 1] == TRUE);
  }

  test_that("logicals value method") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // value() returns the actual value, not a proxy
    cpp4r::r_bool v0 = x.value(static_cast<R_xlen_t>(0));
    cpp4r::r_bool v1 = x.value(static_cast<R_xlen_t>(1));
    cpp4r::r_bool v2 = x.value(static_cast<R_xlen_t>(2));

    expect_true(v0 == TRUE);
    expect_true(v1 == FALSE);
    expect_true(v2 == NA_LOGICAL);
  }

  test_that("logicals set_elt direct") {
    cpp4r::writable::logicals x({FALSE, FALSE, FALSE});

    // Modify via proxy
    x[0] = TRUE;
    x[1] = NA_LOGICAL;

    expect_true(x[0] == TRUE);
    expect_true(x[1] == NA_LOGICAL);
    expect_true(x[2] == FALSE);
  }

  // ============================================
  // Direct tests for get_const_p and get_p
  // ============================================

  test_that("logicals get_p returns pointer") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});

    // Access via data() and LOGICAL
    SEXP data = x.data();
    int* p = LOGICAL(data);

    expect_true(p != nullptr);
    expect_true(p[0] == TRUE);
    expect_true(p[1] == FALSE);
    expect_true(p[2] == TRUE);
  }

  test_that("logicals read from SEXP exercises get_elt") {
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, 5));
    LOGICAL(s)[0] = TRUE;
    LOGICAL(s)[1] = FALSE;
    LOGICAL(s)[2] = NA_LOGICAL;
    LOGICAL(s)[3] = TRUE;
    LOGICAL(s)[4] = FALSE;

    cpp4r::logicals x(s);

    // Each access goes through get_elt
    expect_true(x[0] == TRUE);
    expect_true(x[1] == FALSE);
    expect_true(x[2] == NA_LOGICAL);
    expect_true(x[3] == TRUE);
    expect_true(x[4] == FALSE);

    UNPROTECT(1);
  }

  test_that("logicals writable proxy operator== with r_bool") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // This specifically tests the operator== in logicals.hpp
    // operator==(const r_vector<r_bool>::proxy& lhs, r_bool rhs)
    cpp4r::r_bool t = TRUE;
    cpp4r::r_bool f = FALSE;
    cpp4r::r_bool na = NA_LOGICAL;

    expect_true(x[0] == t);
    expect_true(x[1] == f);
    expect_true(x[2] == na);

    // Also test inequality
    expect_true(!(x[0] == f));
    expect_true(!(x[1] == t));
  }

  // ============================================
  // Additional coverage tests for logicals.hpp
  // ============================================

  test_that("logicals get_sexptype returns LGLSXP") {
    // Verify the SEXPTYPE by creating and checking
    cpp4r::writable::logicals x{TRUE};
    SEXP s = x.data();
    expect_true(TYPEOF(s) == LGLSXP);
  }

  test_that("logicals get_elt accessed via operator[] on read-only") {
    // Create SEXP directly and wrap
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, 4));
    LOGICAL(s)[0] = TRUE;
    LOGICAL(s)[1] = FALSE;
    LOGICAL(s)[2] = NA_LOGICAL;
    LOGICAL(s)[3] = TRUE;

    cpp4r::logicals x(s);

    // Each operator[] access goes through get_elt
    expect_true(x[0] == TRUE);
    expect_true(x[1] == FALSE);
    expect_true(x[2] == NA_LOGICAL);
    expect_true(x[3] == TRUE);

    UNPROTECT(1);
  }

  test_that("logicals get_p for writable non-ALTREP") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});

    // Access underlying data pointer (exercises get_p path)
    int* p = x.data_ptr_writable();
    expect_true(p != nullptr);
    expect_true(p[0] == TRUE);
    expect_true(p[1] == FALSE);
    expect_true(p[2] == TRUE);
  }

  test_that("logicals get_const_p for read-only non-ALTREP") {
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, 3));
    LOGICAL(s)[0] = TRUE;
    LOGICAL(s)[1] = FALSE;
    LOGICAL(s)[2] = TRUE;

    cpp4r::logicals x(s);

    // Access const pointer (exercises get_const_p path)
    const int* p = x.data_ptr();
    expect_true(p != nullptr);
    expect_true(p[0] == TRUE);
    expect_true(p[1] == FALSE);
    expect_true(p[2] == TRUE);

    UNPROTECT(1);
  }

  test_that("logicals set_elt via writable proxy assignment") {
    cpp4r::writable::logicals x({FALSE, FALSE, FALSE, FALSE});

    // Each assignment exercises SET_LOGICAL_ELT via set_elt
    x[0] = TRUE;
    x[1] = FALSE;
    x[2] = NA_LOGICAL;
    x[3] = TRUE;

    expect_true(x[0] == TRUE);
    expect_true(x[1] == FALSE);
    expect_true(x[2] == NA_LOGICAL);
    expect_true(x[3] == TRUE);
  }

  test_that("logicals use_buf returns true for ALTREP-like iteration") {
    // Create a larger vector to trigger buffer usage
    R_xlen_t n = 300;
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      LOGICAL(s)[i] = (i % 2 == 0) ? TRUE : FALSE;
    }

    cpp4r::logicals x(s);

    // Iterate and count - this exercises the iterator buffer logic
    R_xlen_t count_true = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      if (*it == TRUE) ++count_true;
    }

    expect_true(count_true == 150);  // Every other element is TRUE

    UNPROTECT(1);
  }

  test_that("logicals get_region via iterator on large vector") {
    // Create vector > BUF_THRESHOLD to trigger get_region
    R_xlen_t n = 400;
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      LOGICAL(s)[i] = (i % 3 == 0) ? TRUE : FALSE;
    }

    cpp4r::logicals x(s);

    // Use const iterator which triggers buffered reads
    R_xlen_t count = 0;
    R_xlen_t true_count = 0;
    for (auto cit = x.cbegin(); cit != x.cend(); ++cit) {
      if (*cit == TRUE) ++true_count;
      ++count;
    }

    expect_true(count == n);
    // 0, 3, 6, ..., 399 -> 400/3 + 1 = 134 values
    expect_true(true_count == 134);

    UNPROTECT(1);
  }

  test_that("logicals writable::operator== returns correct results") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // Test the inline operator== in writable namespace
    // This is: inline bool operator==(const r_vector<r_bool>::proxy& lhs, r_bool rhs)

    // Compare proxy with r_bool directly using the operator
    cpp4r::r_bool true_val(TRUE);
    cpp4r::r_bool false_val(FALSE);
    cpp4r::r_bool na_val(NA_LOGICAL);

    // These comparisons use the operator== from logicals.hpp line 60
    bool cmp1 = (x[0] == true_val);
    bool cmp2 = (x[1] == false_val);
    bool cmp3 = (x[2] == na_val);

    expect_true(cmp1);
    expect_true(cmp2);
    expect_true(cmp3);

    // Negative cases
    bool cmp4 = (x[0] == false_val);
    bool cmp5 = (x[1] == true_val);

    expect_true(!cmp4);
    expect_true(!cmp5);
  }

  test_that("logicals proxy to r_bool conversion for equality") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL, TRUE, FALSE});

    // Access proxy, convert to r_bool, then compare
    for (R_xlen_t i = 0; i < x.size(); ++i) {
      cpp4r::r_bool val = static_cast<cpp4r::r_bool>(x[i]);
      bool matches = (x[i] == val);
      expect_true(matches);
    }
  }

  test_that("logicals iterator operator* returns r_bool") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    auto it = x.begin();

    cpp4r::r_bool v0 = *it;
    expect_true(v0 == TRUE);

    ++it;
    cpp4r::r_bool v1 = *it;
    expect_true(v1 == FALSE);

    ++it;
    cpp4r::r_bool v2 = *it;
    expect_true(v2 == NA_LOGICAL);
  }

  test_that("logicals direct LOGICAL_ELT access") {
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, 3));
    SET_LOGICAL_ELT(s, 0, TRUE);
    SET_LOGICAL_ELT(s, 1, FALSE);
    SET_LOGICAL_ELT(s, 2, NA_LOGICAL);

    cpp4r::logicals x(s);

    // Verify get_elt is correctly reading LOGICAL_ELT values
    expect_true(x[0] == TRUE);
    expect_true(x[1] == FALSE);
    expect_true(x[2] == NA_LOGICAL);

    UNPROTECT(1);
  }

  test_that("logicals initializer_list constructor") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE, FALSE});

    expect_true(x.size() == 4);
    expect_true(x[0] == TRUE);
    expect_true(x[1] == FALSE);
    expect_true(x[2] == TRUE);
    expect_true(x[3] == FALSE);
  }

  test_that("logicals push_back exercises set_elt") {
    cpp4r::writable::logicals x;
    x.reserve(5);

    x.push_back(TRUE);
    x.push_back(FALSE);
    x.push_back(NA_LOGICAL);
    x.push_back(TRUE);
    x.push_back(FALSE);

    expect_true(x.size() == 5);
    expect_true(x[0] == TRUE);
    expect_true(x[1] == FALSE);
    expect_true(x[2] == NA_LOGICAL);
    expect_true(x[3] == TRUE);
    expect_true(x[4] == FALSE);
  }

  test_that("logicals data_ptr returns LOGICAL pointer") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE});

    // data_ptr_writable returns the underlying int* for logicals
    int* ptr = x.data_ptr_writable();

    if (ptr != nullptr) {
      // Modify via pointer
      ptr[0] = FALSE;
      ptr[1] = TRUE;

      // Verify changes
      expect_true(x[0] == FALSE);
      expect_true(x[1] == TRUE);
    }
  }

  test_that("logicals iteration exercises all iterator paths") {
    // Small vector (no buffering)
    cpp4r::writable::logicals small({TRUE, FALSE});
    R_xlen_t small_count = 0;
    for (auto it = small.begin(); it != small.end(); ++it) {
      ++small_count;
    }
    expect_true(small_count == 2);

    // Medium vector (at threshold)
    R_xlen_t med_n = 256;
    cpp4r::writable::logicals medium(med_n);
    for (R_xlen_t i = 0; i < med_n; ++i) {
      medium[i] = (i % 2 == 0);
    }
    R_xlen_t med_count = 0;
    for (auto it = medium.begin(); it != medium.end(); ++it) {
      ++med_count;
    }
    expect_true(med_count == med_n);

    // Large vector (buffering active)
    R_xlen_t large_n = 512;
    cpp4r::writable::logicals large(large_n);
    for (R_xlen_t i = 0; i < large_n; ++i) {
      large[i] = (i % 2 == 0);
    }
    R_xlen_t large_count = 0;
    for (auto it = large.begin(); it != large.end(); ++it) {
      ++large_count;
    }
    expect_true(large_count == large_n);
  }

  // ============================================
  // Explicit tests for writable::operator==
  // ============================================

  test_that("writable logicals proxy operator== explicit call") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // Get proxies
    auto proxy0 = x[0];
    auto proxy1 = x[1];
    auto proxy2 = x[2];

    // Create r_bool values
    cpp4r::r_bool true_val(TRUE);
    cpp4r::r_bool false_val(FALSE);
    cpp4r::r_bool na_val(NA_LOGICAL);

    // Test operator== from logicals.hpp writable namespace
    // This tests: operator==(const r_vector<r_bool>::proxy& lhs, r_bool rhs)
    bool result1 = (proxy0 == true_val);
    bool result2 = (proxy1 == false_val);
    bool result3 = (proxy2 == na_val);

    expect_true(result1);
    expect_true(result2);
    expect_true(result3);

    // Test negative cases
    bool result4 = (proxy0 == false_val);
    bool result5 = (proxy1 == true_val);
    bool result6 = (proxy0 == na_val);

    expect_true(!result4);
    expect_true(!result5);
    expect_true(!result6);
  }

  test_that("writable logicals proxy comparison in conditional") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE, FALSE, NA_LOGICAL});

    // Use proxy == r_bool in conditional statements
    R_xlen_t true_count = 0;
    R_xlen_t false_count = 0;
    R_xlen_t na_count = 0;

    cpp4r::r_bool t(TRUE);
    cpp4r::r_bool f(FALSE);
    cpp4r::r_bool na(NA_LOGICAL);

    for (R_xlen_t i = 0; i < x.size(); ++i) {
      if (x[i] == t) ++true_count;
      if (x[i] == f) ++false_count;
      if (x[i] == na) ++na_count;
    }

    expect_true(true_count == 2);
    expect_true(false_count == 2);
    expect_true(na_count == 1);
  }

  test_that("writable logicals proxy operator== with different r_bool constructions") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // r_bool from bool
    cpp4r::r_bool from_true(true);
    cpp4r::r_bool from_false(false);

    // r_bool from int
    cpp4r::r_bool from_int_1(1);
    cpp4r::r_bool from_int_0(0);
    cpp4r::r_bool from_int_na(NA_INTEGER);

    // Test comparisons
    expect_true(x[0] == from_true);
    expect_true(x[0] == from_int_1);
    expect_true(x[1] == from_false);
    expect_true(x[1] == from_int_0);
  }
}
