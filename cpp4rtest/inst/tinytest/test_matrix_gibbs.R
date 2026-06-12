# Tests for matrix Gibbs sampler and matrix operations

local({
  set.seed(123)
  result <- gibbs_cpp_(10L, 5L)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(10L, 2L))
  expect_equal(typeof(result), "double")
})

local({
  set.seed(123)
  result <- gibbs_cpp2_(10L, 5L)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(10L, 2L))
  expect_equal(typeof(result), "double")
})

local({
  set.seed(123)
  result1 <- gibbs_cpp_(10L, 5L)
  set.seed(123)
  result2 <- gibbs_cpp2_(10L, 5L)
  expect_equal(result1, result2)
})

local({
  x <- matrix(as.double(1:4), nrow = 2, ncol = 2)
  y <- matrix(as.double(5:8), nrow = 2, ncol = 2)
  result <- matrix_add_(x, y)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(2L, 2L))
  expect_equal(result, x + y)
})

local({
  x <- matrix(as.double(1:100), nrow = 10, ncol = 10)
  y <- matrix(as.double(101:200), nrow = 10, ncol = 10)
  expect_equal(matrix_add_(x, y), x + y)
})
