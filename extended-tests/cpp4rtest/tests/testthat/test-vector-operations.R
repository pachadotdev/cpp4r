# Tests for insert.h, truncate.h, grow.h functions

# insert.h tests
test_that("cpp4r_insert_ works", {
  result <- cpp4r_insert_(5L)
  expect_length(result, 5)

  expect_type(result, "double")
  # Values are inserted at position 0, so they should be in reverse order
  expect_equal(as.numeric(result), c(4, 3, 2, 1, 0))
})

test_that("cpp4r_insert_ with zero", {

  result <- cpp4r_insert_(0L)
  expect_length(result, 0)
})

# truncate.h tests
test_that("cpp4r_push_and_truncate_ works", {
  result <- cpp4r_push_and_truncate_(5L)
  expect_length(result, 6)
  expect_type(result, "double")
  # Last element should be the pushed value (0)
  expect_equal(result[6], 0)
})

# grow.h tests
test_that("grow_ works", {
  result <- grow_(10L)
  expect_length(result, 10)
  expect_type(result, "double")
  expect_equal(as.numeric(result), 0:9)
})

test_that("grow_ with zero returns empty vector", {
  result <- grow_(0L)
  expect_length(result, 0)
  expect_type(result, "double")
})
