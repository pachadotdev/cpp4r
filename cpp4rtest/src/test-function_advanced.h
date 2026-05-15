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

  // --- function vs sexp: "silly" side-by-side comparisons ---

  test_that("function: default constructor gives R_NilValue, same as sexp") {
    // sexp has had a default constructor forever; function now has one too
    cpp4r::sexp s;
    cpp4r::function f;
    expect_true(static_cast<SEXP>(s) == R_NilValue);
    expect_true(static_cast<SEXP>(f) == R_NilValue);
  }

  test_that("function: construct from sexp avoids per-call cast") {
    // sexp approach: must re-wrap on every call
    cpp4r::sexp func_as_sexp = cpp4r::package("base")["sum"];
    double r1 = cpp4r::function{static_cast<SEXP>(func_as_sexp)}(
        cpp4r::as_sexp({1., 2.}));

    // function approach: construct once from sexp, call directly
    cpp4r::function f(func_as_sexp);
    double r2 = f(cpp4r::as_sexp({1., 2.}));

    expect_true(r1 == r2);
  }

  test_that("function: data() round-trips back to sexp") {
    auto sum = cpp4r::package("base")["sum"];
    cpp4r::sexp s = sum.data();
    cpp4r::function f(s);
    double result = f(cpp4r::as_sexp({4., 5., 6.}));
    expect_true(result == 15.);
  }

  test_that("function: copy works like sexp copy") {
    auto f1 = cpp4r::package("base")["sum"];
    cpp4r::function f2(f1);
    double r1 = f1(cpp4r::as_sexp({1., 2.}));
    double r2 = f2(cpp4r::as_sexp({1., 2.}));
    expect_true(r1 == r2);
  }

  test_that("function: move leaves source as R_NilValue, like sexp") {
    // sexp move semantics for reference
    cpp4r::sexp s1 = cpp4r::package("base")["sum"];
    cpp4r::sexp s2 = std::move(s1);
    expect_true(static_cast<SEXP>(s1) == R_NilValue);

    // function has the same semantics
    cpp4r::function f1(cpp4r::package("base")["sum"]);
    cpp4r::function f2(std::move(f1));
    expect_true(static_cast<SEXP>(f1) == R_NilValue);
    double result = f2(cpp4r::as_sexp({1., 2.}));
    expect_true(result == 3.);
  }

  test_that("function: default-construct then assign (class member pattern)") {
    // Previously you had to use sexp for this; now function works the same way
    cpp4r::sexp s;
    s = cpp4r::package("base")["sum"];
    double r1 = cpp4r::function{static_cast<SEXP>(s)}(cpp4r::as_sexp({10., 20.}));

    cpp4r::function f;
    f = cpp4r::package("base")["sum"];
    double r2 = f(cpp4r::as_sexp({10., 20.}));

    expect_true(r1 == r2);
    expect_true(r2 == 30.);
  }

  test_that("function: callback struct with function member vs sexp member") {
    // cpp11 style: sexp member, must cast on every invocation
    struct SexpCallback {
      cpp4r::sexp func;
      explicit SexpCallback(SEXP f) : func(f) {}
      cpp4r::sexp invoke(cpp4r::sexp x) const {
        return cpp4r::function{static_cast<SEXP>(func)}(x);
      }
    };

    // cpp4r style: function member, store and call directly
    struct FuncCallback {
      cpp4r::function func;
      explicit FuncCallback(SEXP f) : func(f) {}
      cpp4r::sexp invoke(cpp4r::sexp x) const { return func(x); }
    };

    SEXP sum_sexp = cpp4r::package("base")["sum"];
    cpp4r::sexp args = cpp4r::as_sexp({1., 2., 3.});
    double r1 = SexpCallback(sum_sexp).invoke(args);
    double r2 = FuncCallback(sum_sexp).invoke(args);
    expect_true(r1 == r2);
    expect_true(r1 == 6.);
  }
}
