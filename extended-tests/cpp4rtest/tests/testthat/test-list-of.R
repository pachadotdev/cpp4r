# Tests for list_of operations

test_that("list_of<doubles> works", {
  result <- cpp4r_list_of_doubles_()
  expect_type(result, "list")
  expect_length(result, 3)
  expect_equal(result[[1]], c(1.0, 2.0))
  expect_equal(result[[2]], c(3.0, 4.0, 5.0))
  expect_equal(result[[3]], c(6.0))
})

test_that("list_of<integers> works", {
  result <- cpp4r_list_of_integers_()
  expect_type(result, "list")
  expect_length(result, 2)
  expect_equal(result[[1]], c(1L, 2L, 3L))
  expect_equal(result[[2]], c(4L, 5L))
})

test_that("list_of<strings> works", {
  result <- cpp4r_list_of_strings_()
  expect_type(result, "list")
  expect_length(result, 2)
  expect_equal(result[[1]], c("hello", "world"))
  expect_equal(result[[2]], c("foo", "bar", "baz"))
})

test_that("list_of with named elements works", {
  result <- cpp4r_list_of_named_()
  expect_type(result, "list")
  expect_equal(names(result), c("a", "b"))
})
