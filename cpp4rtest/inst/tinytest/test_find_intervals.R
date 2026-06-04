# Tests for find-intervals.h functions

local({
  x <- c(1.0, 2.0, 3.0)
  result <- remove_altrep(x)
  tinytest::expect_equal(typeof(result), "double")
  tinytest::expect_equal(length(result), length(x))
})

local({
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  tinytest::expect_equal(upper_bound(2.5, breaks), 2)
  tinytest::expect_equal(upper_bound(0.5, breaks), 0)
  tinytest::expect_equal(upper_bound(5.5, breaks), 5)
})

local({
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  tinytest::expect_equal(findInterval2(x, breaks), c(0L, 1L, 2L, 3L, 4L, 5L))
})

local({
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  tinytest::expect_equal(findInterval2_5(x, breaks), c(0L, 1L, 2L, 3L, 4L, 5L))
})

local({
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  tinytest::expect_equal(findInterval3(x, breaks), c(0L, 1L, 2L, 3L, 4L, 5L))
})

local({
  breaks <- c(1.0, 2.0, 3.0, 4.0, 5.0)
  x <- c(0.5, 1.5, 2.5, 3.5, 4.5, 5.5)
  base_result <- findInterval(x, breaks)
  tinytest::expect_equal(findInterval2(x, breaks), base_result)
  tinytest::expect_equal(findInterval2_5(x, breaks), base_result)
  tinytest::expect_equal(findInterval3(x, breaks), base_result)
})
