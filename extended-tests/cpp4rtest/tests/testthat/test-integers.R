# Tests for integer vector operations

test_that("integer vector basic operations work", {
  x <- c(1L, 2L, 3L, 4L, 5L)
  result <- cpp4r_add_int_vec_(x, 10L)
  expect_equal(as.integer(result), c(11L, 12L, 13L, 14L, 15L))
})

test_that("integer vector handles empty input", {
  x <- integer(0)
  result <- cpp4r_add_int_vec_(x, 5L)
  expect_length(result, 0)
})

test_that("integer vector handles NA values", {
  x <- c(1L, NA_integer_, 3L)
  result <- cpp4r_add_int_vec_(x, 1L)
  expect_true(is.na(result[2]))
})

test_that("integer conversion from doubles works", {
  x <- c(1.0, 2.0, 3.0)
  result <- cpp4r_as_integers_(x)
  expect_type(result, "integer")
  expect_equal(as.integer(result), c(1L, 2L, 3L))
})

test_that("integer conversion rejects non-integerish", {
  x <- c(1.5, 2.5, 3.5)
  expect_error(cpp4r_as_integers_(x))
})

test_that("integer vector large input", {
  n <- 10000L
  x <- seq_len(n)
  result <- cpp4r_add_int_vec_(x, 1L)
  expect_length(result, n)
  expect_equal(result[1], 2L)
  expect_equal(result[n], n + 1L)
})
