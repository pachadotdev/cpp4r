# Tests for iterator operations via R interface

test_that("iterator sum works (doubles)", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  result <- cpp4r_iterator_sum_(x)
  expect_equal(result, 15.0)
})

test_that("iterator sum works (integers)", {
  x <- c(1L, 2L, 3L, 4L, 5L)
  result <- cpp4r_iterator_sum_int_(x)
  expect_equal(result, 15L)
})

test_that("iterator find works", {
  x <- c(10.0, 20.0, 30.0, 40.0, 50.0)
  
  # Find existing value
  result <- cpp4r_iterator_find_(x, 30.0)
  expect_equal(result, 3L)  # 1-indexed position
  
  # Find non-existing value
  result <- cpp4r_iterator_find_(x, 99.0)
  expect_equal(result, -1L)
})

test_that("iterator count works", {
  x <- c(1.0, 2.0, 2.0, 3.0, 2.0, 4.0)
  result <- cpp4r_iterator_count_(x, 2.0)
  expect_equal(result, 3L)
})

test_that("iterator min/max works", {
  x <- c(5.0, 2.0, 8.0, 1.0, 9.0, 3.0)
  
  min_val <- cpp4r_iterator_min_(x)
  expect_equal(min_val, 1.0)
  
  max_val <- cpp4r_iterator_max_(x)
  expect_equal(max_val, 9.0)
})

test_that("iterator transform works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  result <- cpp4r_iterator_double_values_(x)
  expect_equal(as.numeric(result), c(2.0, 4.0, 6.0, 8.0, 10.0))
})

test_that("iterator with empty vector", {
  x <- numeric(0)
  result <- cpp4r_iterator_sum_(x)
  expect_equal(result, 0.0)
})

test_that("iterator with single element", {
  x <- 42.0
  result <- cpp4r_iterator_sum_(x)
  expect_equal(result, 42.0)
})

test_that("iterator with large vector", {
  n <- 10000
  x <- as.double(seq_len(n))
  result <- cpp4r_iterator_sum_(x)
  expected <- n * (n + 1) / 2
  expect_equal(result, expected)
})

test_that("reverse iterator works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  result <- cpp4r_reverse_vector_(x)
  expect_equal(as.numeric(result), c(5.0, 4.0, 3.0, 2.0, 1.0))
})

test_that("iterator arithmetic works", {
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  
  # Get element at index 2 (0-based)
  result <- cpp4r_iterator_at_(x, 2L)
  expect_equal(result, 3.0)
  
  # Get distance between begin and end
  dist <- cpp4r_iterator_distance_(x)
  expect_equal(dist, 5L)
})
