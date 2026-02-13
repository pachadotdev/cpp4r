# Tests for raw byte operations

test_that("raw vector creation works", {
  x <- as.raw(c(0, 1, 2, 255))
  result <- cpp4r_raw_copy_(x)
  expect_equal(result, x)
})

test_that("raw vector empty input", {
  x <- raw(0)
  result <- cpp4r_raw_copy_(x)
  expect_length(result, 0)
})

test_that("raw vector modification", {
  x <- as.raw(c(0, 128, 255))
  result <- cpp4r_raw_xor_(x, as.raw(0xFF))
  expect_equal(result, as.raw(c(255, 127, 0)))
})
