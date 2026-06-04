# Tests for sum.h and sum_int.h functions

local({
  x <- 1L:10L
  tinytest::expect_equal(sum_int_for_(x), 55)
  tinytest::expect_equal(sum_int_for2_(x), 55)
  tinytest::expect_equal(sum_int_foreach_(x), 55)
  tinytest::expect_equal(sum_int_accumulate_(x), 55)
})

local({
  x <- 1L:100L
  base_sum <- sum(x)
  tinytest::expect_equal(sum_int_for_(x), base_sum)
  tinytest::expect_equal(sum_int_for2_(x), base_sum)
  tinytest::expect_equal(sum_int_foreach_(x), base_sum)
  tinytest::expect_equal(sum_int_accumulate_(x), base_sum)
})

local({
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  tinytest::expect_equal(sum_dbl_for_(x), 15.0)
  tinytest::expect_equal(sum_dbl_for2_(x), 15.0)
  tinytest::expect_equal(sum_dbl_for3_(x), 15.0)
  tinytest::expect_equal(sum_dbl_foreach_(x), 15.0)
  tinytest::expect_equal(sum_dbl_foreach2_(x), 15.0)
  tinytest::expect_equal(sum_dbl_accumulate_(x), 15.0)
  tinytest::expect_equal(sum_dbl_accumulate2_(x), 15.0)
})

local({
  x <- as.double(1:100)
  base_sum <- sum(x)
  tinytest::expect_equal(sum_dbl_for_(x), base_sum)
  tinytest::expect_equal(sum_dbl_for2_(x), base_sum)
  tinytest::expect_equal(sum_dbl_for3_(x), base_sum)
  tinytest::expect_equal(sum_dbl_foreach_(x), base_sum)
  tinytest::expect_equal(sum_dbl_foreach2_(x), base_sum)
  tinytest::expect_equal(sum_dbl_accumulate_(x), base_sum)
  tinytest::expect_equal(sum_dbl_accumulate2_(x), base_sum)
})
