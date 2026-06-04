# Tests for insert.h, truncate.h, grow.h functions

local({
  result <- insert_(5L)
  tinytest::expect_equal(length(result), 5)
  tinytest::expect_equal(typeof(result), "double")
  tinytest::expect_equal(as.numeric(result), c(4, 3, 2, 1, 0))
})

local({
  result <- insert_(0L)
  tinytest::expect_equal(length(result), 0)
})

local({
  result <- push_and_truncate_(5L)
  tinytest::expect_equal(length(result), 6)
  tinytest::expect_equal(typeof(result), "double")
  tinytest::expect_equal(result[6], 0)
})

local({
  result <- grow_(10L)
  tinytest::expect_equal(length(result), 10)
  tinytest::expect_equal(typeof(result), "double")
  tinytest::expect_equal(as.numeric(result), 0:9)
})

local({
  result <- grow_(0L)
  tinytest::expect_equal(length(result), 0)
  tinytest::expect_equal(typeof(result), "double")
})
