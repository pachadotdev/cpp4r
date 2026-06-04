# Tests for integer vector operations

local({
  x <- c(1L, 2L, 3L, 4L, 5L)
  result <- add_int_vec_(x, 10L)
  expect_equal(as.integer(result), c(11L, 12L, 13L, 14L, 15L))
})

local({
  x <- integer(0)
  result <- add_int_vec_(x, 5L)
  expect_equal(length(result), 0)
})

local({
  x <- c(1L, NA_integer_, 3L)
  result <- add_int_vec_(x, 1L)
  expect_true(is.na(result[2]))
})

local({
  x <- c(1.0, 2.0, 3.0)
  result <- as_integers_(x)
  expect_equal(typeof(result), "integer")
  expect_equal(as.integer(result), c(1L, 2L, 3L))
})

expect_error(as_integers_(c(1.5, 2.5, 3.5)))

local({
  n <- 10000L
  x <- seq_len(n)
  result <- add_int_vec_(x, 1L)
  expect_equal(length(result), n)
  expect_equal(result[1], 2L)
  expect_equal(result[n], n + 1L)
})
