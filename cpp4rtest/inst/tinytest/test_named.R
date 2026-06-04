# Tests for named argument operations

local({
  result <- named_doubles_()
  tinytest::expect_true(!is.null(names(result)))
  tinytest::expect_equal(names(result), c("a", "b", "c"))
  tinytest::expect_equal(as.numeric(result), c(1.0, 2.0, 3.0))
})

local({
  result <- named_integers_()
  tinytest::expect_true(!is.null(names(result)))
  tinytest::expect_equal(names(result), c("x", "y", "z"))
  tinytest::expect_equal(as.integer(result), c(10L, 20L, 30L))
})

local({
  result <- named_strings_()
  tinytest::expect_true(!is.null(names(result)))
  tinytest::expect_equal(names(result), c("first", "second", "third"))
  tinytest::expect_equal(as.character(result), c("one", "two", "three"))
})

local({
  result <- named_list_()
  tinytest::expect_true(!is.null(names(result)))
  tinytest::expect_equal(names(result), c("int", "dbl", "str"))
})

local({
  x <- c(a = 1.0, b = 2.0, c = 3.0)
  tinytest::expect_equal(get_by_name_(x, "a"), 1.0)
  tinytest::expect_equal(get_by_name_(x, "b"), 2.0)
  tinytest::expect_equal(get_by_name_(x, "c"), 3.0)
})

local({
  x <- c(foo = 1.0, bar = 2.0)
  tinytest::expect_true(contains_name_(x, "foo"))
  tinytest::expect_true(contains_name_(x, "bar"))
  tinytest::expect_false(contains_name_(x, "baz"))
})

local({
  x <- c(alpha = 10.0, beta = 20.0, gamma = 30.0)
  tinytest::expect_equal(find_name_pos_(x, "beta"), 2L)
  tinytest::expect_equal(find_name_pos_(x, "delta"), -1L)
})
