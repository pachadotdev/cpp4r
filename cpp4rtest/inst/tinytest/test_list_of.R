# Tests for list_of operations

local({
  result <- list_of_doubles_()
  tinytest::expect_equal(typeof(result), "list")
  tinytest::expect_equal(length(result), 3)
  tinytest::expect_equal(result[[1]], c(1.0, 2.0))
  tinytest::expect_equal(result[[2]], c(3.0, 4.0, 5.0))
  tinytest::expect_equal(result[[3]], c(6.0))
})

local({
  result <- list_of_integers_()
  tinytest::expect_equal(typeof(result), "list")
  tinytest::expect_equal(length(result), 2)
  tinytest::expect_equal(result[[1]], c(1L, 2L, 3L))
  tinytest::expect_equal(result[[2]], c(4L, 5L))
})

local({
  result <- list_of_strings_()
  tinytest::expect_equal(typeof(result), "list")
  tinytest::expect_equal(length(result), 2)
  tinytest::expect_equal(result[[1]], c("hello", "world"))
  tinytest::expect_equal(result[[2]], c("foo", "bar", "baz"))
})

local({
  result <- list_of_named_()
  tinytest::expect_equal(typeof(result), "list")
  tinytest::expect_equal(names(result), c("a", "b"))
})
