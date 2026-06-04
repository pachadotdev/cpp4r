# Tests for nullable external pointer

local({
  len <- 1e5
  set.seed(42)
  x <- rnorm(len)

  expect_equal(nullable_extptr_1(), NULL)
  expect_equal(nullable_extptr_2(), NULL)
})
