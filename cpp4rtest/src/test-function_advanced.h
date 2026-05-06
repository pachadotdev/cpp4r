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

  test_that("function::try_call() succeeds and returns value") {
    auto sum = cpp4r::package("base")["sum"];
    cpp4r::call_result res = sum.try_call(cpp4r::as_sexp({1., 2., 3.}));

    expect_false(res.error);
    expect_true(static_cast<bool>(res));
    expect_true(res.error_message() == nullptr);

    double val = cpp4r::as_cpp<cpp4r::doubles>(res.value)[0];
    expect_true(val == 6.);
  }

  test_that("function::try_call() captures R errors silently") {
    auto log = cpp4r::package("base")["log"];
    // log("not a number") causes an R error
    cpp4r::call_result res = log.try_call(cpp4r::as_sexp(std::string("x")));

    expect_true(res.error);
    expect_false(static_cast<bool>(res));
    expect_true(res.error_message() != nullptr);
  }

  test_that("function::call_in() evaluates in given environment") {
    cpp4r::environment e = cpp4r::environment::new_env();
    e["x"] = 42;

    // Create a call to `get("x")` and evaluate it in our custom env
    auto get_fn = cpp4r::package("base")["get"];
    sexp result = get_fn.call_in(e, cpp4r::as_sexp(std::string("x")));

    expect_true(cpp4r::as_cpp<cpp4r::integers>(result)[0] == 42);
  }

  test_that("function::try_call_in() fails silently in given environment") {
    cpp4r::environment e = cpp4r::environment::new_env();
    // "y" is not defined in e

    auto get_fn = cpp4r::package("base")["get"];
    cpp4r::call_result res = get_fn.try_call_in(e, cpp4r::as_sexp(std::string("y")));

    expect_true(res.error);
  }
}
