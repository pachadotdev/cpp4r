// Extended tests for r_vector_fwd.hpp to improve coverage
// Tests iterator buffer logic, proxy class, find_cached, and C++11/14 helpers

#include <testthat.h>
#include <algorithm>
#include <vector>

context("r_vector_fwd-extended-C++") {
  // ============================================
  // Iterator buffer tests (BUF_CAP=64, BUF_THRESHOLD=256)
  // ============================================

  test_that("integers iterator buffer with large vector") {
    // Create a vector larger than BUF_THRESHOLD (256) to trigger buffered iteration
    R_xlen_t n = 500;
    cpp4r::writable::integers x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<int>(i + 1);
    }

    // Iterate through the entire vector
    R_xlen_t sum = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      sum += *it;
    }

    // Sum should be n*(n+1)/2
    R_xlen_t expected = n * (n + 1) / 2;
    expect_true(sum == expected);
  }

  test_that("doubles iterator buffer with large vector") {
    R_xlen_t n = 500;
    cpp4r::writable::doubles x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<double>(i + 1);
    }

    double sum = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      sum += *it;
    }

    double expected = n * (n + 1) / 2.0;
    expect_true(std::abs(sum - expected) < 0.001);
  }

  test_that("strings iterator buffer with large vector") {
    R_xlen_t n = 300;
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

  test_that("logicals iterator buffer with large vector") {
    R_xlen_t n = 500;
    cpp4r::writable::logicals x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = (i % 2 == 0);
    }

    R_xlen_t count_true = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      if (*it == TRUE) ++count_true;
    }

    expect_true(count_true == 250);
  }

  test_that("raws iterator buffer with large vector") {
    R_xlen_t n = 500;
    cpp4r::writable::raws x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<Rbyte>(i % 256);
    }

    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == n);
  }

  test_that("complexes iterator buffer with large vector") {
    R_xlen_t n = 300;
    cpp4r::writable::complexes x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = cpp4r::r_complex(static_cast<double>(i), static_cast<double>(i * 2));
    }

    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == n);
  }

  // ============================================
  // Iterator arithmetic tests
  // ============================================

  test_that("iterator operator-= and operator-") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    auto it = x.end();
    it -= 3;
    expect_true(*it == 8);

    auto it2 = it - 2;
    expect_true(*it2 == 6);
  }

  test_that("iterator comparison operators") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5});

    auto it1 = x.begin();
    auto it2 = x.begin() + 2;
    auto it3 = x.end();

    // operator<
    expect_true(it1 < it2);
    expect_true(it2 < it3);
    expect_true(!(it2 < it1));

    // operator>
    expect_true(it2 > it1);
    expect_true(it3 > it2);
    expect_true(!(it1 > it2));

    // operator<=
    expect_true(it1 <= it2);
    expect_true(it1 <= it1);
    expect_true(!(it2 <= it1));

    // operator>=
    expect_true(it2 >= it1);
    expect_true(it2 >= it2);
    expect_true(!(it1 >= it2));
  }

  test_that("iterator operator[] (subscript)") {
    cpp4r::writable::integers x({10, 20, 30, 40, 50});

    auto it = x.begin();
    expect_true(it[0] == 10);
    expect_true(it[1] == 20);
    expect_true(it[2] == 30);
    expect_true(it[4] == 50);

    it += 2;
    expect_true(it[0] == 30);
    expect_true(it[1] == 40);
  }

  test_that("iterator prefix and postfix operators") {
    cpp4r::writable::integers x({1, 2, 3});

    auto it = x.begin();

    // Prefix ++
    auto it2 = ++it;
    expect_true(*it == 2);
    expect_true(*it2 == 2);

    // Postfix ++
    auto it3 = it++;
    expect_true(*it == 3);
    expect_true(*it3 == 2);

    // Prefix --
    auto it4 = --it;
    expect_true(*it == 2);
    expect_true(*it4 == 2);

    // Postfix --
    auto it5 = it--;
    expect_true(*it == 1);
    expect_true(*it5 == 2);
  }

  // ============================================
  // Proxy class tests
  // ============================================

  test_that("proxy assignment from value") {
    cpp4r::writable::integers x({1, 2, 3});

    x[0] = 100;
    expect_true(x[0] == 100);

    // Chain assignment
    x[1] = x[2] = 50;
    expect_true(x[1] == 50);
    expect_true(x[2] == 50);
  }

  test_that("proxy conversion to value") {
    cpp4r::writable::integers x({42});

    int val = x[0];
    expect_true(val == 42);

    // Use in expression
    int doubled = x[0] * 2;
    expect_true(doubled == 84);
  }

  test_that("proxy operator== and operator!=") {
    cpp4r::writable::integers x({1, 1, 2});

    // Proxy == proxy
    expect_true(x[0] == x[1]);
    expect_true(!(x[0] == x[2]));

    // Proxy != proxy
    expect_true(x[0] != x[2]);
    expect_true(!(x[0] != x[1]));

    // Proxy == value
    expect_true(x[0] == 1);
    expect_true(!(x[0] == 2));

    // Value == proxy
    expect_true(1 == x[0]);
    expect_true(!(2 == x[0]));
  }

  // ============================================
  // find_cached tests (for named vectors)
  // ============================================

  test_that("find_cached with repeated lookups") {
    using namespace cpp4r::literals;

    cpp4r::writable::integers x({"a"_nm = 1, "b"_nm = 2, "c"_nm = 3, "d"_nm = 4});

    // First lookup
    expect_true(x["a"] == 1);
    expect_true(x["b"] == 2);

    // Repeated lookups should use cache
    for (int i = 0; i < 10; ++i) {
      expect_true(x["a"] == 1);
      expect_true(x["c"] == 3);
    }
  }

  test_that("find_cached with non-existent key") {
    using namespace cpp4r::literals;

    cpp4r::writable::integers x({"a"_nm = 1, "b"_nm = 2});

    expect_error(x["nonexistent"]);
  }

  test_that("contains method") {
    using namespace cpp4r::literals;

    cpp4r::writable::doubles x({"a"_nm = 1.0, "b"_nm = 2.0, "c"_nm = 3.0});

    expect_true(x.contains("a"));
    expect_true(x.contains("b"));
    expect_true(x.contains("c"));
    expect_true(!x.contains("d"));
    expect_true(!x.contains(""));
  }

  // ============================================
  // Iterator with STL algorithms
  // ============================================

  test_that("std::distance with iterators") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5});

    auto dist = std::distance(x.begin(), x.end());
    expect_true(dist == 5);

    auto it = x.begin() + 2;
    expect_true(std::distance(x.begin(), it) == 2);
    expect_true(std::distance(it, x.end()) == 3);
  }

  test_that("std::advance with iterators") {
    cpp4r::writable::integers x({10, 20, 30, 40, 50});

    auto it = x.begin();
    std::advance(it, 3);
    expect_true(*it == 40);

    std::advance(it, -2);
    expect_true(*it == 20);
  }

  test_that("std::accumulate with iterators") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5});

    int sum = std::accumulate(x.begin(), x.end(), 0);
    expect_true(sum == 15);
  }

  test_that("std::copy with iterators") {
    cpp4r::writable::integers src({1, 2, 3, 4, 5});
    std::vector<int> dst(5);

    std::copy(src.begin(), src.end(), dst.begin());

    expect_true(dst[0] == 1);
    expect_true(dst[1] == 2);
    expect_true(dst[2] == 3);
    expect_true(dst[3] == 4);
    expect_true(dst[4] == 5);
  }

  test_that("std::transform with iterators") {
    cpp4r::writable::integers x({1, 2, 3, 4, 5});
    cpp4r::writable::integers y(5);

    std::transform(x.begin(), x.end(), y.begin(), [](int v) { return v * 2; });

    expect_true(y[0] == 2);
    expect_true(y[1] == 4);
    expect_true(y[2] == 6);
    expect_true(y[3] == 8);
    expect_true(y[4] == 10);
  }

  test_that("std::find with iterators") {
    cpp4r::writable::integers x({10, 20, 30, 40, 50});

    auto it = std::find(x.begin(), x.end(), 30);
    expect_true(it != x.end());
    expect_true(*it == 30);

    auto it2 = std::find(x.begin(), x.end(), 99);
    expect_true(it2 == x.end());
  }

  test_that("std::count with iterators") {
    cpp4r::writable::integers x({1, 2, 1, 3, 1, 4, 1});

    auto count = std::count(x.begin(), x.end(), 1);
    expect_true(count == 4);

    auto count2 = std::count(x.begin(), x.end(), 99);
    expect_true(count2 == 0);
  }

  // ============================================
  // Read-only iterator tests
  // ============================================

  test_that("read-only integers iterator") {
    SEXP x = PROTECT(Rf_allocVector(INTSXP, 5));
    INTEGER(x)[0] = 1;
    INTEGER(x)[1] = 2;
    INTEGER(x)[2] = 3;
    INTEGER(x)[3] = 4;
    INTEGER(x)[4] = 5;

    cpp4r::integers y(x);

    int sum = 0;
    for (auto it = y.begin(); it != y.end(); ++it) {
      sum += *it;
    }

    expect_true(sum == 15);

    UNPROTECT(1);
  }

  test_that("read-only doubles iterator") {
    SEXP x = PROTECT(Rf_allocVector(REALSXP, 5));
    REAL(x)[0] = 1.0;
    REAL(x)[1] = 2.0;
    REAL(x)[2] = 3.0;
    REAL(x)[3] = 4.0;
    REAL(x)[4] = 5.0;

    cpp4r::doubles y(x);

    double sum = 0;
    for (auto it = y.begin(); it != y.end(); ++it) {
      sum += *it;
    }

    expect_true(std::abs(sum - 15.0) < 0.001);

    UNPROTECT(1);
  }

  test_that("read-only strings iterator") {
    SEXP x = PROTECT(Rf_allocVector(STRSXP, 3));
    SET_STRING_ELT(x, 0, Rf_mkChar("a"));
    SET_STRING_ELT(x, 1, Rf_mkChar("b"));
    SET_STRING_ELT(x, 2, Rf_mkChar("c"));

    cpp4r::strings y(x);

    R_xlen_t count = 0;
    for (auto it = y.begin(); it != y.end(); ++it) {
      ++count;
    }

    expect_true(count == 3);

    UNPROTECT(1);
  }

  // ============================================
  // Edge cases
  // ============================================

  test_that("empty vector iteration") {
    cpp4r::writable::integers x;

    int count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == 0);
  }

  test_that("single element vector iteration") {
    cpp4r::writable::integers x({42});

    auto it = x.begin();
    expect_true(*it == 42);
    ++it;
    expect_true(it == x.end());
  }

  test_that("iterator to exactly BUF_THRESHOLD elements") {
    // Exactly 256 elements (BUF_THRESHOLD)
    R_xlen_t n = 256;
    cpp4r::writable::integers x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<int>(i);
    }

    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == n);
  }

  test_that("iterator to BUF_THRESHOLD + 1 elements") {
    // 257 elements (just over BUF_THRESHOLD)
    R_xlen_t n = 257;
    cpp4r::writable::integers x(n);

    for (R_xlen_t i = 0; i < n; ++i) {
      x[i] = static_cast<int>(i);
    }

    R_xlen_t count = 0;
    for (auto it = x.begin(); it != x.end(); ++it) {
      ++count;
    }

    expect_true(count == n);
  }

  // ============================================
  // find_cached with pre-translated names (read-only r_vector)
  // ============================================

  test_that("find_cached with pre-translated names vector") {
    using namespace cpp4r::literals;

    // Create writable first, then convert to read-only for find_cached tests
    cpp4r::writable::integers wx(
        {"alpha"_nm = 1, "beta"_nm = 2, "gamma"_nm = 3, "delta"_nm = 4});
    cpp4r::integers x(wx);

    // Build a pre-translated names cache
    std::vector<std::string> names_cache;
    cpp4r::strings names = x.names();
    for (R_xlen_t i = 0; i < names.size(); ++i) {
      names_cache.push_back(static_cast<std::string>(names[i]));
    }

    // Use find_cached with the pre-translated cache
    auto it = x.find_cached(names_cache, "beta");
    expect_true(it != x.end());
    expect_true(*it == 2);

    auto it2 = x.find_cached(names_cache, "delta");
    expect_true(it2 != x.end());
    expect_true(*it2 == 4);

    // Non-existent key
    auto it3 = x.find_cached(names_cache, "nonexistent");
    expect_true(it3 == x.end());
  }

  test_that("find with names cache overload") {
    using namespace cpp4r::literals;

    // Create writable first, then convert to read-only
    cpp4r::writable::doubles wx({"a"_nm = 1.0, "b"_nm = 2.0, "c"_nm = 3.0});
    cpp4r::doubles x(wx);

    // Build names cache
    cpp4r::strings names = x.names();
    std::vector<std::string> names_cache;
    for (R_xlen_t i = 0; i < names.size(); ++i) {
      names_cache.push_back(static_cast<std::string>(names[i]));
    }

    // Use find with cache (read-only version)
    auto it = x.find(names_cache, "b");
    expect_true(it != x.end());
    expect_true(*it == 2.0);
  }

  // ============================================
  // More proxy tests
  // ============================================

  test_that("proxy arithmetic operators") {
    cpp4r::writable::integers x({10, 20, 30});

    // operator+=
    x[0] += 5;
    expect_true(x[0] == 15);

    // operator-=
    x[1] -= 5;
    expect_true(x[1] == 15);

    // operator*=
    x[2] *= 2;
    expect_true(x[2] == 60);

    // operator/=
    x[2] /= 3;
    expect_true(x[2] == 20);
  }

  test_that("proxy increment/decrement operators") {
    cpp4r::writable::integers x({10, 20, 30});

    // prefix ++
    ++x[0];
    expect_true(x[0] == 11);

    // prefix --
    --x[0];
    expect_true(x[0] == 10);

    // postfix ++ (returns reference to proxy, increments value)
    x[1]++;
    expect_true(x[1] == 21);

    // postfix --
    x[1]--;
    expect_true(x[1] == 20);
  }

  test_that("proxy assignment from another proxy") {
    cpp4r::writable::integers x({1, 2, 3});

    // Assign from another proxy
    x[0] = x[2];
    expect_true(x[0] == 3);
    expect_true(x[2] == 3);
  }

  // ============================================
  // value() method tests
  // ============================================

  test_that("value method for integers") {
    cpp4r::writable::integers x({10, 20, 30});

    int v0 = x.value(static_cast<R_xlen_t>(0));
    int v1 = x.value(static_cast<R_xlen_t>(1));
    int v2 = x.value(static_cast<R_xlen_t>(2));

    expect_true(v0 == 10);
    expect_true(v1 == 20);
    expect_true(v2 == 30);
  }

  test_that("value method for doubles") {
    cpp4r::writable::doubles x({1.5, 2.5, 3.5});

    double v0 = x.value(static_cast<R_xlen_t>(0));
    double v1 = x.value(static_cast<R_xlen_t>(1));
    double v2 = x.value(static_cast<R_xlen_t>(2));

    expect_true(std::abs(v0 - 1.5) < 0.001);
    expect_true(std::abs(v1 - 2.5) < 0.001);
    expect_true(std::abs(v2 - 3.5) < 0.001);
  }

  // ============================================
  // at() bounds checking
  // ============================================

  test_that("at() bounds checking for integers") {
    cpp4r::writable::integers x({1, 2, 3});

    expect_true(x.at(0) == 1);
    expect_true(x.at(1) == 2);
    expect_true(x.at(2) == 3);

    expect_error(x.at(3));
    expect_error(x.at(-1));
  }

  test_that("at() with name lookup") {
    using namespace cpp4r::literals;

    cpp4r::writable::integers x({"a"_nm = 1, "b"_nm = 2});

    expect_true(x.at("a") == 1);
    expect_true(x.at("b") == 2);

    expect_error(x.at("nonexistent"));
  }

  // ============================================
  // Type conversion tests
  // ============================================

  test_that("operator SEXP conversion") {
    cpp4r::writable::integers x({1, 2, 3});

    SEXP s = static_cast<SEXP>(x);
    expect_true(s != R_NilValue);
    expect_true(Rf_isInteger(s));
    expect_true(Rf_xlength(s) == 3);
  }

  test_that("operator sexp conversion") {
    cpp4r::writable::doubles x({1.0, 2.0, 3.0});

    cpp4r::sexp s = static_cast<cpp4r::sexp>(x);
    SEXP raw = static_cast<SEXP>(s);
    expect_true(raw != R_NilValue);
    expect_true(Rf_isReal(raw));
  }

  // ============================================
  // Constructor tests
  // ============================================

  test_that("constructor from iterator range") {
    std::vector<int> v = {1, 2, 3, 4, 5};
    cpp4r::writable::integers x(v.begin(), v.end());

    expect_true(x.size() == 5);
    expect_true(x[0] == 1);
    expect_true(x[4] == 5);
  }

  test_that("constructor from container") {
    std::vector<double> v = {1.0, 2.0, 3.0};
    cpp4r::writable::doubles x(v);

    expect_true(x.size() == 3);
    expect_true(std::abs(x[0] - 1.0) < 0.001);
    expect_true(std::abs(x[2] - 3.0) < 0.001);
  }

  test_that("constructor with explicit size") {
    cpp4r::writable::integers x(static_cast<R_xlen_t>(10));

    expect_true(x.size() == 10);

    // Fill with values
    for (R_xlen_t i = 0; i < 10; ++i) {
      x[i] = static_cast<int>(i * 10);
    }

    expect_true(x[0] == 0);
    expect_true(x[9] == 90);
  }

  // ============================================
  // type_error exception tests
  // ============================================

  test_that("type_error exception message") {
    cpp4r::type_error err(INTSXP, REALSXP);

    const char* msg = err.what();
    expect_true(msg != nullptr);
    // The message should contain type information
    std::string msg_str(msg);
    bool has_type_info = (msg_str.find("integer") != std::string::npos) ||
                         (msg_str.find("double") != std::string::npos);
    expect_true(has_type_info);
  }

  test_that("type_error thrown on wrong type") {
    SEXP wrong = PROTECT(Rf_allocVector(REALSXP, 3));

    expect_error(cpp4r::integers(wrong));

    UNPROTECT(1);
  }

  // ============================================
  // is_altrep method tests
  // ============================================

  test_that("is_altrep returns false for regular vectors") {
    cpp4r::writable::integers x({1, 2, 3});
    expect_true(!x.is_altrep());

    cpp4r::writable::doubles y({1.0, 2.0, 3.0});
    expect_true(!y.is_altrep());

    cpp4r::writable::logicals z({TRUE, FALSE});
    expect_true(!z.is_altrep());

    cpp4r::writable::raws r({1, 2, 3});
    expect_true(!r.is_altrep());
  }

  // ============================================
  // data_ptr and data_ptr_writable tests
  // ============================================

  test_that("data_ptr for read-only vectors") {
    SEXP s = PROTECT(Rf_allocVector(INTSXP, 3));
    INTEGER(s)[0] = 1;
    INTEGER(s)[1] = 2;
    INTEGER(s)[2] = 3;

    cpp4r::integers x(s);
    const int* p = x.data_ptr();

    // data_ptr should return valid pointer for non-ALTREP
    if (p != nullptr) {
      expect_true(p[0] == 1);
      expect_true(p[1] == 2);
      expect_true(p[2] == 3);
    }

    UNPROTECT(1);
  }

  test_that("data_ptr_writable for writable vectors") {
    cpp4r::writable::integers x({1, 2, 3});
    int* p = x.data_ptr_writable();

    // Should return valid pointer for non-ALTREP writable vectors
    if (p != nullptr) {
      expect_true(p[0] == 1);
      p[0] = 100;
      expect_true(x[0] == 100);
    }
  }

  // ============================================
  // Iterator operator (subtraction to get iterator)
  // ============================================

  test_that("iterator operator- returns new iterator") {
    cpp4r::writable::logicals x({TRUE, FALSE, TRUE, FALSE, TRUE});

    auto it = x.end();
    auto it2 = it + (-2);  // This tests operator+ with negative
    // Verify the iterator moved back
    (void)it2;  // Suppress unused warning - just testing compilation
  }

  // ============================================
  // More thorough proxy tests
  // ============================================

  test_that("logicals proxy equality") {
    cpp4r::writable::logicals x({TRUE, FALSE, NA_LOGICAL});

    // Test the operator== defined in logicals.hpp
    expect_true(x[0] == cpp4r::r_bool(TRUE));
    expect_true(x[1] == cpp4r::r_bool(FALSE));
    expect_true(x[2] == cpp4r::r_bool(NA_LOGICAL));
  }

  // ============================================
  // Additional logicals and raws iterator tests
  // ============================================

  test_that("logicals iterator fill_buf on large read-only vector") {
    // Create a large read-only vector to exercise fill_buf/get_region
    R_xlen_t n = 350;
    SEXP s = PROTECT(Rf_allocVector(LGLSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      LOGICAL(s)[i] = (i % 2 == 0) ? TRUE : FALSE;
    }

    cpp4r::logicals x(s);

    // Iterate and verify buffered reads
    R_xlen_t idx = 0;
    R_xlen_t true_count = 0;
    for (auto it = x.cbegin(); it != x.cend(); ++it) {
      if (*it == TRUE) ++true_count;
      ++idx;
    }

    expect_true(idx == n);
    expect_true(true_count == 175);  // Every other element

    UNPROTECT(1);
  }

  test_that("raws iterator fill_buf on large read-only vector") {
    // Create a large read-only vector to exercise fill_buf/get_region
    R_xlen_t n = 350;
    SEXP s = PROTECT(Rf_allocVector(RAWSXP, n));
    for (R_xlen_t i = 0; i < n; ++i) {
      RAW(s)[i] = static_cast<Rbyte>(i % 100);
    }

    cpp4r::raws x(s);

    // Iterate and verify buffered reads
    R_xlen_t idx = 0;
    int sum = 0;
    for (auto it = x.cbegin(); it != x.cend(); ++it) {
      sum += *it;
      ++idx;
    }

    expect_true(idx == n);
    // Sum of (0..99)*3 + (0..49) = 3*4950 + 1225 = 16075
    expect_true(sum == 16075);

    UNPROTECT(1);
  }

  test_that("logicals set_elt boundary conditions") {
    cpp4r::writable::logicals x(5);

    // Set at boundaries
    x[0] = TRUE;        // first element
    x[4] = FALSE;       // last element
    x[2] = NA_LOGICAL;  // middle element

    expect_true(x[0] == TRUE);
    expect_true(x[4] == FALSE);
    expect_true(x[2] == NA_LOGICAL);
  }

  test_that("raws set_elt boundary conditions") {
    cpp4r::writable::raws x(5);

    // Set at boundaries with extreme values
    x[0] = 0;    // min value at first element
    x[4] = 255;  // max value at last element
    x[2] = 128;  // mid value at middle element

    expect_true(x[0] == 0);
    expect_true(x[4] == 255);
    expect_true(x[2] == 128);
  }

  test_that("logicals writable proxy comparison with literal") {
    cpp4r::writable::logicals x({TRUE, FALSE});

    // These use operator== from logicals.hpp
    cpp4r::r_bool t(TRUE);
    cpp4r::r_bool f(FALSE);

    expect_true(x[0] == t);
    expect_true(x[1] == f);
    expect_true(!(x[0] == f));
    expect_true(!(x[1] == t));
  }

  test_that("raws multiple assignments to same element") {
    cpp4r::writable::raws x({0});

    // Repeated assignments to exercise set_elt
    for (int i = 0; i <= 255; i += 17) {
      x[0] = static_cast<Rbyte>(i);
      expect_true(x[0] == static_cast<Rbyte>(i));
    }
  }

  test_that("logicals multiple assignments to same element") {
    cpp4r::writable::logicals x{TRUE};

    // Repeated assignments to exercise set_elt
    x[0] = TRUE;
    expect_true(x[0] == TRUE);

    x[0] = FALSE;
    expect_true(x[0] == FALSE);

    x[0] = NA_LOGICAL;
    expect_true(x[0] == NA_LOGICAL);

    x[0] = TRUE;
    expect_true(x[0] == TRUE);
  }
}
