# Tests for safe.h functions

local({
  x <- c(1.0, 2.0, 3.0)
  result <- safe_(x)
  tinytest::expect_equal(typeof(result), "double")
  tinytest::expect_equal(length(result), 1)
})

local({
  x <- 42.0
  result <- safe_(x)
  tinytest::expect_equal(typeof(result), "double")
  tinytest::expect_equal(length(result), 1)
})

local({
  x <- as.double(1:10)
  result <- safe_(x)
  tinytest::expect_true(is.double(result))
})
