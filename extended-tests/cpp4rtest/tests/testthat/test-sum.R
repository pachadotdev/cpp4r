# Tests for sum.h and sum_int.h functions

# sum_int.h tests
test_that("sum_int_for_ works", {
  x <- 1L:10L
  expect_equal(sum_int_for_(x), 55)
})

test_that("sum_int_for2_ works", {
  x <- 1L:10L
  expect_equal(sum_int_for2_(x), 55)
})

test_that("sum_int_foreach_ works", {
  x <- 1L:10L
  expect_equal(sum_int_foreach_(x), 55)
})

test_that("sum_int_accumulate_ works", {
  x <- 1L:10L
  expect_equal(sum_int_accumulate_(x), 55)
})

test_that("integer sum functions match base R sum", {
  x <- 1L:100L
  base_sum <- sum(x)
  expect_equal(sum_int_for_(x), base_sum)
  expect_equal(sum_int_for2_(x), base_sum)
  expect_equal(sum_int_foreach_(x), base_sum)
  expect_equal(sum_int_accumulate_(x), base_sum)
})

# sum.h tests (doubles)
test_that("sum_dbl_for_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_for_(x), 15.0)
})

test_that("sum_dbl_for2_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_for2_(x), 15.0)
})

test_that("sum_dbl_for3_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_for3_(x), 15.0)
})

test_that("sum_dbl_foreach_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_foreach_(x), 15.0)
})

test_that("sum_dbl_foreach2_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_foreach2_(x), 15.0)
})

test_that("sum_dbl_accumulate_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_accumulate_(x), 15.0)
})

test_that("sum_dbl_accumulate2_ works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  expect_equal(sum_dbl_accumulate2_(x), 15.0)
})

test_that("double sum functions match base R sum", {
  x <- as.double(1:100)
  base_sum <- sum(x)
  expect_equal(sum_dbl_for_(x), base_sum)
  expect_equal(sum_dbl_for2_(x), base_sum)
  expect_equal(sum_dbl_for3_(x), base_sum)
  expect_equal(sum_dbl_foreach_(x), base_sum)
  expect_equal(sum_dbl_foreach2_(x), base_sum)
  expect_equal(sum_dbl_accumulate_(x), base_sum)
  expect_equal(sum_dbl_accumulate2_(x), base_sum)
})
