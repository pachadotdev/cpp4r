# Tests for named argument operations

local({
  result <- named_doubles_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("a", "b", "c"))
  expect_equal(as.numeric(result), c(1.0, 2.0, 3.0))
})

local({
  result <- named_integers_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("x", "y", "z"))
  expect_equal(as.integer(result), c(10L, 20L, 30L))
})

local({
  result <- named_strings_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("first", "second", "third"))
  expect_equal(as.character(result), c("one", "two", "three"))
})

local({
  result <- named_list_()
  expect_true(!is.null(names(result)))
  expect_equal(names(result), c("int", "dbl", "str"))
})

local({
  x <- c(a = 1.0, b = 2.0, c = 3.0)
  expect_equal(get_by_name_(x, "a"), 1.0)
  expect_equal(get_by_name_(x, "b"), 2.0)
  expect_equal(get_by_name_(x, "c"), 3.0)
})

local({
  x <- c(foo = 1.0, bar = 2.0)
  expect_true(contains_name_(x, "foo"))
  expect_true(contains_name_(x, "bar"))
  expect_false(contains_name_(x, "baz"))
})

local({
  x <- c(alpha = 10.0, beta = 20.0, gamma = 30.0)
  expect_equal(find_name_pos_(x, "beta"), 2L)
  expect_equal(find_name_pos_(x, "delta"), -1L)
})
