# Tests for protect.h and release.h functions

tinytest::expect_no_error <- function(expr) {
  result <- tryCatch(expr, error = \(e) e)
  tinytest::expect_false(inherits(result, "error"))
}

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_rapi_(x, 10L)
  tinytest::expect_equal(x, c(1.0, 2.0, 3.0))
})

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_sexp_(x, 10L)
  tinytest::expect_equal(x, c(1.0, 2.0, 3.0))
})

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_(x, 10L)
  tinytest::expect_equal(x, c(1.0, 2.0, 3.0))
})

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_preserve_(x, 10L)
  tinytest::expect_equal(x, c(1.0, 2.0, 3.0))
})

tinytest::expect_no_error(protect_many_rapi_(10L))
tinytest::expect_no_error(protect_many_(10L))
tinytest::expect_no_error(protect_many_sexp_(10L))
tinytest::expect_no_error(protect_many_preserve_(10L))

tinytest::expect_no_error(release_(10L))
tinytest::expect_no_error(release_(100L))
