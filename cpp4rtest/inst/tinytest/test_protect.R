# Tests for protect.h and release.h functions

expect_no_error <- function(expr) {
  result <- tryCatch(expr, error = \(e) e)
  expect_false(inherits(result, "error"))
}

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_rapi_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_sexp_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

local({
  x <- c(1.0, 2.0, 3.0)
  protect_one_preserve_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

expect_no_error(protect_many_rapi_(10L))
expect_no_error(protect_many_(10L))
expect_no_error(protect_many_sexp_(10L))
expect_no_error(protect_many_preserve_(10L))

expect_no_error(release_(10L))
expect_no_error(release_(100L))
