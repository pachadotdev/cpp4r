# Tests for find-intervals.h functions

test_that("remove_altrep works", {
  x <- c(1.0, 2.0, 3.0)
  result <- remove_altrep(x)
  # Function copies doubles - verify type and length
  expect_type(result, "double")
  expect_length(result, length(x))
})

test_that("upper_bound works", {
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  # upper_bound returns 0-indexed position of first element > x
  expect_equal(upper_bound(2.5, breaks), 2)  # position after 2.0
  expect_equal(upper_bound(0.5, breaks), 0)  # before all elements
  expect_equal(upper_bound(5.5, breaks), 5)  # after all elements
})

test_that("findInterval2 works", {
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  result <- findInterval2(x, breaks)
  expect_equal(result, c(0L, 1L, 2L, 3L, 4L, 5L))
})

test_that("findInterval2_5 works", {
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  result <- findInterval2_5(x, breaks)
  expect_equal(result, c(0L, 1L, 2L, 3L, 4L, 5L))
})

test_that("findInterval3 works", {
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  result <- findInterval3(x, breaks)
  expect_equal(result, c(0L, 1L, 2L, 3L, 4L, 5L))
})

test_that("findInterval functions match base R findInterval", {
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  base_result <- findInterval(x, breaks)

  expect_equal(findInterval2(x, breaks), base_result)
  expect_equal(findInterval2_5(x, breaks), base_result)
  expect_equal(findInterval3(x, breaks), base_result)
})
