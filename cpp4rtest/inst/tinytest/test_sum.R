# Tests for sum.h and sum_int.h functions

local({
  x <- 1L:10L
  expect_equal(sum_int_for_(x), 55)
  expect_equal(sum_int_sexp_for_(x), 55)
  expect_equal(sum_int_foreach_(x), 55)
  expect_equal(sum_int_accumulate_(x), 55)
})

local({
  x <- 1L:100L
  base_sum <- sum(x)
  expect_equal(sum_int_for_(x), base_sum)
  expect_equal(sum_int_sexp_for_(x), base_sum)
  expect_equal(sum_int_foreach_(x), base_sum)
  expect_equal(sum_int_accumulate_(x), base_sum)
})

local({
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_for_(x), 15.0)
  expect_equal(sum_dbl_sexp_for_(x), 15.0)
  expect_equal(sum_dbl_sexp_writable_for_(x), 15.0)
  expect_equal(sum_dbl_foreach_(x), 15.0)
  expect_equal(sum_dbl_sexp_foreach_(x), 15.0)
  expect_equal(sum_dbl_accumulate_(x), 15.0)
  expect_equal(sum_dbl_sexp_accumulate_(x), 15.0)
})

local({
  x <- as.double(1:100)
  base_sum <- sum(x)
  expect_equal(sum_dbl_for_(x), base_sum)
  expect_equal(sum_dbl_sexp_for_(x), base_sum)
  expect_equal(sum_dbl_sexp_writable_for_(x), base_sum)
  expect_equal(sum_dbl_foreach_(x), base_sum)
  expect_equal(sum_dbl_sexp_foreach_(x), base_sum)
  expect_equal(sum_dbl_accumulate_(x), base_sum)
  expect_equal(sum_dbl_sexp_accumulate_(x), base_sum)
})
