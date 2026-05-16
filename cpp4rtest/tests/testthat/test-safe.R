# Tests for safe.h functions

test_that("safe_ works", {
  x <- c(1.0, 2.0, 3.0)
  result <- safe_(x)
  expect_type(result, "double")
  expect_length(result, 1)
})

test_that("safe_ handles single element", {
  x <- 42.0
  result <- safe_(x)
  expect_type(result, "double")
  expect_length(result, 1)
})

test_that("safe_ returns allocated vector via unwind_protect", {
  x <- as.double(1:10)
  result <- safe_(x)
  # The function allocates a REALSXP of length 1 via unwind_protect
  expect_true(is.double(result))
})
