# Tests for logical vector operations

local({
  x <- c(TRUE, FALSE, TRUE)
  expect_equal(as.logical(negate_logical_(x)), c(FALSE, TRUE, FALSE))
})

local({
  x <- c(TRUE, NA, FALSE)
  result <- negate_logical_(x)
  expect_true(is.na(result[2]))
})

local({
  x <- logical(0)
  expect_equal(length(negate_logical_(x)), 0)
})

local({
  x <- rep(TRUE, 100)
  expect_true(all(negate_logical_(x) == FALSE))
})

local({
  x <- rep(FALSE, 100)
  expect_true(all(negate_logical_(x) == TRUE))
})

local({
  x <- c(TRUE, FALSE, TRUE, FALSE)
  result <- logical_to_int_(x)
  expect_equal(typeof(result), "integer")
  expect_equal(as.integer(result), c(1L, 0L, 1L, 0L))
})

local({
  x <- c(TRUE, FALSE, TRUE)
  result <- logical_to_dbl_(x)
  expect_equal(typeof(result), "double")
  expect_equal(as.numeric(result), c(1.0, 0.0, 1.0))
})
