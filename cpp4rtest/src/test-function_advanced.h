#include <testthat.h>

context("function-advanced-C++") {
  test_that("function with multiple named arguments") {
    using namespace cpp4r::literals;

    auto paste = cpp4r::package("base")["paste"];
    std::string res = cpp4r::as_cpp<std::string>(paste("a", "b", "c", "sep"_nm = "-"));
    expect_true(res == "a-b-c");
  }

  test_that("package::operator[] with std::string") {
    std::string func_name = "sum";
    auto sum = cpp4r::package("base")[func_name];
    double result = sum(cpp4r::as_sexp({1., 2., 3.}));
    expect_true(result == 6.);
  }

  test_that("cpp4r::message works") {
    // Just test that it doesn't throw - message output is captured by R
    cpp4r::message("test message");
    expect_true(true);
  }

  test_that("cpp4r::message with format args") {
    cpp4r::message("test %s %d", "message", 42);
    expect_true(true);
  }

  test_that("cpp4r::message with std::string") {
    std::string msg = "test message";
    cpp4r::message(msg);
    expect_true(true);
  }
}
