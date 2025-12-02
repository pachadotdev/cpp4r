#include <testthat.h>

context("protect-advanced-C++") {
  test_that("warning() works without throwing") {
    cpp4r::warning("test warning");
    expect_true(true);
  }

  test_that("warning() with format string") {
    cpp4r::warning("test %s %d", "warning", 42);
    expect_true(true);
  }

  test_that("warning() with std::string") {
    std::string msg = "test warning";
    cpp4r::warning(msg);
    expect_true(true);
  }

  test_that("check_user_interrupt doesn't throw normally") {
    cpp4r::check_user_interrupt();
    expect_true(true);
  }

  test_that("preserve list operations are balanced") {
    R_xlen_t before = cpp4r::detail::store::count();

    {
      cpp4r::writable::integers x(10);
      cpp4r::writable::integers y(20);
      R_xlen_t during = cpp4r::detail::store::count();
      expect_true(during - before == 2);
    }

    R_xlen_t after = cpp4r::detail::store::count();
    expect_true(after == before);
  }

  test_that("unwind_protect with void return") {
    int value = 0;
    cpp4r::unwind_protect([&] { value = 42; });
    expect_true(value == 42);
  }

  test_that("unwind_protect with non-SEXP return") {
    int result = cpp4r::unwind_protect([&] { return 42; });
    expect_true(result == 42);
  }
}
