# Tests for matrix.h Gibbs sampler and matrix operations

test_that("gibbs_cpp works", {
  set.seed(123)
  result <- gibbs_cpp(10L, 5L)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(10L, 2L))
  expect_type(result, "double")
})

test_that("gibbs_cpp2 works", {
  set.seed(123)
  result <- gibbs_cpp2(10L, 5L)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(10L, 2L))
  expect_type(result, "double")
})

test_that("gibbs_cpp and gibbs_cpp2 produce same results with same seed", {
  set.seed(123)
  result1 <- gibbs_cpp(10L, 5L)
  set.seed(123)
  result2 <- gibbs_cpp2(10L, 5L)
  expect_equal(result1, result2)
})

test_that("matrix_add works", {
  x <- matrix(as.double(1:4), nrow = 2, ncol = 2)
  y <- matrix(as.double(5:8), nrow = 2, ncol = 2)
  result <- matrix_add(x, y)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(2L, 2L))
  expect_equal(result, x + y)
})

test_that("matrix_add handles larger matrices", {
  x <- matrix(as.double(1:100), nrow = 10, ncol = 10)
  y <- matrix(as.double(101:200), nrow = 10, ncol = 10)
  result <- matrix_add(x, y)
  expect_equal(result, x + y)
})
