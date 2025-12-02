# Tests for named argument operations

test_that("named doubles work", {
  result <- cpp4r_named_doubles_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("a", "b", "c"))
  expect_equal(as.numeric(result), c(1.0, 2.0, 3.0))
})

test_that("named integers work", {
  result <- cpp4r_named_integers_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("x", "y", "z"))
  expect_equal(as.integer(result), c(10L, 20L, 30L))
})

test_that("named strings work", {
  result <- cpp4r_named_strings_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("first", "second", "third"))
  expect_equal(as.character(result), c("one", "two", "three"))
})

test_that("named list works", {
  result <- cpp4r_named_list_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("int", "dbl", "str"))
})

test_that("access by name works", {
  x <- c(a = 1.0, b = 2.0, c = 3.0)
  
  result_a <- cpp4r_get_by_name_(x, "a")
  expect_equal(result_a, 1.0)
  
  result_b <- cpp4r_get_by_name_(x, "b")
  expect_equal(result_b, 2.0)
  
  result_c <- cpp4r_get_by_name_(x, "c")
  expect_equal(result_c, 3.0)
})

test_that("contains name works", {
  x <- c(foo = 1.0, bar = 2.0)
  
  expect_true(cpp4r_contains_name_(x, "foo"))
  expect_true(cpp4r_contains_name_(x, "bar"))
  expect_false(cpp4r_contains_name_(x, "baz"))
})

test_that("find by name works", {
  x <- c(alpha = 10.0, beta = 20.0, gamma = 30.0)
  
  pos <- cpp4r_find_name_pos_(x, "beta")
  expect_equal(pos, 2L)  # 1-indexed
  
  pos_not_found <- cpp4r_find_name_pos_(x, "delta")
  expect_equal(pos_not_found, -1L)
})
