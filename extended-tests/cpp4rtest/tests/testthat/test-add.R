# Tests for add.h functions

test_that("cpp4r_add_vec_for_ works", {
  x <- c(1.0, 2.0, 3.0)
  result <- cpp4r_add_vec_for_(x, 10.0)
  expect_equal(as.numeric(result), c(11.0, 12.0, 13.0))
})

test_that("cpp4r_add_vec_for_ handles empty vector", {
  x <- numeric(0)
  result <- cpp4r_add_vec_for_(x, 5.0)
  expect_length(result, 0)
})

test_that("cpp4r_add_vec_for_ handles negative numbers", {
  x <- c(-1.0, 0.0, 1.0)
  result <- cpp4r_add_vec_for_(x, -5.0)
  expect_equal(as.numeric(result), c(-6.0, -5.0, -4.0))
})
