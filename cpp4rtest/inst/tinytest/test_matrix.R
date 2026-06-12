# Tests for matrix operations

local({
  x <- cbind(x1 = 3, x2 = c(4:1, 2:5))
  expect_equal(row_sums_(x), rowSums(x))

  x[4, 2] <- NA
  expect_equal(row_sums_(x), rowSums(x))

  y <- cbind(x1 = 3, x2 = c(4:1, 2:5))
  y[3, ] <- NA
  expect_equal(row_sums_(y), rowSums(y))
})

local({
  x <- cbind(3, c(4:1, 2:5))
  expect_equal(col_sums_(x), colSums(x))

  x[4, 2] <- NA
  expect_equal(col_sums_(x), colSums(x))

  y <- cbind(3, c(4:1, 2:5))
  y[3, ] <- NA
  expect_equal(col_sums_(y), colSums(y))
})

local({
  x <- matrix(c(1, 2, 3, 4), nrow = 2, ncol = 2)
  colnames(x) <- letters[1:2]
  rownames(x) <- letters[3:4]

  y <- mat_mat_copy_dimnames_(x)
  z <- mat_sexp_copy_dimnames_(x)

  expect_equal(x, y)
  expect_equal(x, z)

  r <- mat_mat_create_dimnames_()
  expect_equal(rownames(r), c("a", "b"))
  expect_equal(colnames(r), c("c", "d"))
})

local({
  x <- matrix(1:6, nrow = 2, ncol = 3)
  y <- matrix(7:12, nrow = 2, ncol = 3)
  result <- matrix_add_coerce_test_(x, y)
  expect_true(is.matrix(result))
  expect_equal(dim(result), c(2L, 3L))
  expect_equal(typeof(result), "double")
  expect_equal(result, x + y)
})

local({
  x <- matrix(1:6, nrow = 2, ncol = 3)
  y <- matrix(as.double(7:12), nrow = 2, ncol = 3)
  result <- matrix_mixed_add_(x, y)
  expect_true(is.matrix(result))
  expect_equal(typeof(result), "double")
  expect_equal(result, x + y)
})

local({
  x <- matrix(c(TRUE, FALSE, TRUE, FALSE, TRUE, FALSE), nrow = 2, ncol = 3)
  y <- matrix(c(FALSE, TRUE, FALSE, TRUE, FALSE, TRUE), nrow = 2, ncol = 3)
  result <- matrix_add_coerce_test_(x, y)
  expect_true(is.matrix(result))
  expect_equal(typeof(result), "double")
  expect_equal(result, matrix(1, nrow = 2, ncol = 3))
})

local({
  x <- matrix(c(1L, NA_integer_, 3L, 4L), nrow = 2, ncol = 2)
  y <- matrix(c(5L, 6L, NA_integer_, 8L), nrow = 2, ncol = 2)
  result <- matrix_add_coerce_test_(x, y)
  expect_true(is.matrix(result))
  expect_equal(typeof(result), "double")
  expect_true(is.na(result[1, 2]))
  expect_true(is.na(result[2, 1]))
  expect_equal(result[1, 1], 6)
  expect_equal(result[2, 2], 12)
})
