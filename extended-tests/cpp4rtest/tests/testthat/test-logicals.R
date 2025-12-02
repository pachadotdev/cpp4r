# Tests for logical vector operations

test_that("logical vector operations work", {
  x <- c(TRUE, FALSE, TRUE)
  result <- cpp4r_negate_logical_(x)
  expect_equal(as.logical(result), c(FALSE, TRUE, FALSE))
})

test_that("logical vector handles NA", {
  x <- c(TRUE, NA, FALSE)
  result <- cpp4r_negate_logical_(x)
  expect_true(is.na(result[2]))
})

test_that("logical vector empty input", {
  x <- logical(0)
  result <- cpp4r_negate_logical_(x)
  expect_length(result, 0)
})

test_that("all true vector", {
  x <- rep(TRUE, 100)
  result <- cpp4r_negate_logical_(x)
  expect_true(all(result == FALSE))
})

test_that("all false vector", {
  x <- rep(FALSE, 100)
  result <- cpp4r_negate_logical_(x)
  expect_true(all(result == TRUE))
})

test_that("logical to integer conversion", {
  x <- c(TRUE, FALSE, TRUE, FALSE)
  result <- cpp4r_logical_to_int_(x)
  expect_type(result, "integer")
  expect_equal(as.integer(result), c(1L, 0L, 1L, 0L))
})

test_that("logical to double conversion", {
  x <- c(TRUE, FALSE, TRUE)
  result <- cpp4r_logical_to_dbl_(x)
  expect_type(result, "double")
  expect_equal(as.numeric(result), c(1.0, 0.0, 1.0))
})
