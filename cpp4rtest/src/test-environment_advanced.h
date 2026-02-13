#include <testthat.h>

context("environment-advanced-C++") {
  test_that("environment::exists() works") {
    auto new_env = cpp4r::package("base")["new.env"];
    cpp4r::environment x(new_env());

    expect_false(x.exists("foo"));
    x["foo"] = 1;
    expect_true(x.exists("foo"));
  }

  test_that("environment::exists() with SEXP symbol") {
    auto new_env = cpp4r::package("base")["new.env"];
    cpp4r::environment x(new_env());

    SEXP sym = Rf_install("bar");
    expect_false(x.exists(sym));
    x["bar"] = 2;
    expect_true(x.exists(sym));
  }

  test_that("environment::exists() with std::string") {
    auto new_env = cpp4r::package("base")["new.env"];
    cpp4r::environment x(new_env());

    std::string name = "baz";
    expect_false(x.exists(name));
    x["baz"] = 3;
    expect_true(x.exists(name));
  }

  test_that("environment::remove() with SEXP symbol") {
    auto new_env = cpp4r::package("base")["new.env"];
    cpp4r::environment x(new_env());

    x["foo"] = 1;
    expect_true(x.exists("foo"));

    SEXP sym = Rf_install("foo");
    x.remove(sym);
    expect_false(x.exists("foo"));
  }
}
