#include <testthat.h>

context("r_string-advanced-C++") {
  test_that("r_string size() method") {
    cpp4r::r_string x("hello");
    expect_true(x.size() == 5);

    cpp4r::r_string empty("");
    expect_true(empty.size() == 0);
  }

  test_that("r_string comparison with std::string") {
    cpp4r::r_string x("hello");
    std::string y = "hello";
    expect_true(x == y);
    expect_false(x == std::string("world"));
  }

  test_that("r_string comparison with SEXP") {
    cpp4r::r_string x("hello");
    SEXP y = Rf_mkCharCE("hello", CE_UTF8);
    expect_true(x == y);
  }

  test_that("r_string conversion to std::string") {
    cpp4r::r_string x("hello world");
    std::string s = static_cast<std::string>(x);
    expect_true(s == "hello world");
  }

  test_that("r_string from std::string preserves content") {
    std::string input = "test string";
    cpp4r::r_string x(input);
    std::string output = static_cast<std::string>(x);
    expect_true(input == output);
  }

  test_that("as_sexp for initializer_list of r_string") {
    cpp4r::r_string a("a");
    cpp4r::r_string b("b");
    SEXP result = cpp4r::as_sexp({a, b});
    expect_true(Rf_xlength(result) == 2);
    expect_true(Rf_isString(result));
  }

  test_that("as_sexp for initializer_list with NA_STRING") {
    cpp4r::r_string na(NA_STRING);
    cpp4r::r_string a("a");
    SEXP result = cpp4r::as_sexp({na, a});
    expect_true(STRING_ELT(result, 0) == NA_STRING);
  }
}
