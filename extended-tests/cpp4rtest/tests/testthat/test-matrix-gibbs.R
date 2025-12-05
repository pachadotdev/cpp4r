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

# Tests for implicit matrix type coercion

test_that("doubles_matrix accepts integer matrices via implicit coercion", {
  # Integer matrices should be automatically coerced to double
  x <- matrix(1:6, nrow = 2, ncol = 3) # integer matrix
  y <- matrix(7:12, nrow = 2, ncol = 3) # integer matrix

  # This should work now without needing storage.mode conversion
  result <- matrix_add_coerce_test(x, y)

  expect_true(is.matrix(result))
  expect_equal(dim(result), c(2L, 3L))
  expect_type(result, "double")
  expect_equal(result, x + y) # R automatically promotes to double
})

test_that("doubles_matrix accepts mixed integer and double matrices", {
  x <- matrix(1:6, nrow = 2, ncol = 3) # integer matrix
  y <- matrix(as.double(7:12), nrow = 2, ncol = 3) # double matrix

  result <- matrix_mixed_add(x, y)

  expect_true(is.matrix(result))
  expect_type(result, "double")
  expect_equal(result, x + y)
})

test_that("doubles_matrix accepts logical matrices via implicit coercion", {
  x <- matrix(c(TRUE, FALSE, TRUE, FALSE, TRUE, FALSE), nrow = 2, ncol = 3)
  y <- matrix(c(FALSE, TRUE, FALSE, TRUE, FALSE, TRUE), nrow = 2, ncol = 3)

  result <- matrix_add_coerce_test(x, y)

  expect_true(is.matrix(result))
  expect_type(result, "double")
  # TRUE = 1, FALSE = 0, so TRUE + FALSE = 1, FALSE + TRUE = 1
  expect_equal(result, matrix(1, nrow = 2, ncol = 3))
})

test_that("matrix coercion handles NA values correctly", {
  x <- matrix(c(1L, NA_integer_, 3L, 4L), nrow = 2, ncol = 2)
  y <- matrix(c(5L, 6L, NA_integer_, 8L), nrow = 2, ncol = 2)

  result <- matrix_add_coerce_test(x, y)

  expect_true(is.matrix(result))
  expect_type(result, "double")
  # NA + anything = NA
  expect_true(is.na(result[1, 2]))
  expect_true(is.na(result[2, 1]))
  expect_equal(result[1, 1], 6) # 1 + 5
  expect_equal(result[2, 2], 12) # 4 + 8
})
