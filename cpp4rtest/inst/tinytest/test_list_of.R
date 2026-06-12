# Tests for list_of operations

local({
  result <- list_of_doubles_()
  expect_equal(typeof(result), "list")
  expect_equal(length(result), 3)
  expect_equal(result[[1]], c(1.0, 2.0))
  expect_equal(result[[2]], c(3.0, 4.0, 5.0))
  expect_equal(result[[3]], c(6.0))
})

local({
  result <- list_of_integers_()
  expect_equal(typeof(result), "list")
  expect_equal(length(result), 2)
  expect_equal(result[[1]], c(1L, 2L, 3L))
  expect_equal(result[[2]], c(4L, 5L))
})

local({
  result <- list_of_strings_()
  expect_equal(typeof(result), "list")
  expect_equal(length(result), 2)
  expect_equal(result[[1]], c("hello", "world"))
  expect_equal(result[[2]], c("foo", "bar", "baz"))
})

local({
  result <- list_of_named_()
  expect_equal(typeof(result), "list")
  expect_equal(names(result), c("a", "b"))
})
