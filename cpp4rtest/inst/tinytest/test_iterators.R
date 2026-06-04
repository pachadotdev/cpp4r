# Tests for iterator operations

local({
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  tinytest::expect_equal(iterator_sum_(x), 15.0)
})

local({
  x <- c(1L, 2L, 3L, 4L, 5L)
  tinytest::expect_equal(iterator_sum_int_(x), 15L)
})

local({
  x <- c(10.0, 20.0, 30.0, 40.0, 50.0)
  tinytest::expect_equal(iterator_find_(x, 30.0), 3L)
  tinytest::expect_equal(iterator_find_(x, 99.0), -1L)
})

local({
  x <- c(1.0, 2.0, 2.0, 3.0, 2.0, 4.0)
  tinytest::expect_equal(iterator_count_(x, 2.0), 3L)
})

local({
  x <- c(5.0, 2.0, 8.0, 1.0, 9.0, 3.0)
  tinytest::expect_equal(iterator_min_(x), 1.0)
  tinytest::expect_equal(iterator_max_(x), 9.0)
})

local({
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  tinytest::expect_equal(as.numeric(iterator_double_values_(x)), c(2.0, 4.0, 6.0, 8.0, 10.0))
})

tinytest::expect_equal(iterator_sum_(numeric(0)), 0.0)
tinytest::expect_equal(iterator_sum_(42.0), 42.0)

local({
  n <- 10000
  x <- as.double(seq_len(n))
  tinytest::expect_equal(iterator_sum_(x), n * (n + 1) / 2)
})

local({
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  tinytest::expect_equal(as.numeric(reverse_vector_(x)), c(5.0, 4.0, 3.0, 2.0, 1.0))
})

local({
  x <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  tinytest::expect_equal(iterator_at_(x, 2L), 3.0)
  tinytest::expect_equal(iterator_distance_(x), 5L)
})
