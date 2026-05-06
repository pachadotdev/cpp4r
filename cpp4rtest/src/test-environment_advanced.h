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

  test_that("environment::new_env() creates an empty environment") {
    cpp4r::environment e = cpp4r::environment::new_env();

    expect_true(TYPEOF(SEXP(e)) == ENVSXP);
    expect_false(e.exists("x"));
    expect_true(e.size() == 0);
  }

  test_that("environment::new_env() respects enclosing environment") {
    cpp4r::environment parent = cpp4r::environment::new_env();
    parent["x"] = 42;

    cpp4r::environment child = cpp4r::environment::new_env(parent);

    // x is in parent's frame, not child's own frame
    expect_false(child.exists("x"));
    expect_true(parent.exists("x"));
  }

  test_that("environment::global_env() wraps R_GlobalEnv") {
    cpp4r::environment g = cpp4r::environment::global_env();
    expect_true(SEXP(g) == R_GlobalEnv);
  }

  test_that("environment::base_env() wraps R_BaseEnv") {
    cpp4r::environment b = cpp4r::environment::base_env();
    expect_true(SEXP(b) == R_BaseEnv);
  }

  test_that("environment::empty_env() wraps R_EmptyEnv") {
    cpp4r::environment e = cpp4r::environment::empty_env();
    expect_true(SEXP(e) == R_EmptyEnv);
  }

  test_that("environment::lock() prevents new bindings") {
    cpp4r::environment e = cpp4r::environment::new_env();

    expect_false(e.is_locked());
    e.lock();
    expect_true(e.is_locked());
  }

  test_that("environment::ls() returns sorted binding names") {
    cpp4r::environment e = cpp4r::environment::new_env();
    e["z"] = 1;
    e["a"] = 2;
    e["m"] = 3;

    cpp4r::strings names(e.ls());

    expect_true(names.size() == 3);
    expect_true(names[0] == "a");
    expect_true(names[1] == "m");
    expect_true(names[2] == "z");
  }

  test_that("environment::ls() respects all_names for dot names") {
    cpp4r::environment e = cpp4r::environment::new_env();
    e[".hidden"] = 1;
    e["visible"] = 2;

    cpp4r::strings without_dots(e.ls(false));
    expect_true(without_dots.size() == 1);
    expect_true(without_dots[0] == "visible");

    cpp4r::strings with_dots(e.ls(true));
    expect_true(with_dots.size() == 2);
  }
}
