# Tests for doubles iterators

local({
  len <- 1e5
  set.seed(42)
  x <- rnorm(len)
  sum_base <- sum(x)

  expect_equal(sum_dbl_for_(x), sum_base)
  expect_equal(sum_dbl_foreach_(x), sum_base)
  expect_equal(sum_dbl_accumulate_(x), sum_base)
  expect_equal(sum_dbl_sexp_for_(x), sum_base)
})

local({
  len <- 1e5
  x <- as.double(seq_len(len))
  sum_base <- sum(x)

  expect_equal(sum_dbl_for_(x), sum_base)
  expect_equal(sum_dbl_foreach_(x), sum_base)
  expect_equal(sum_dbl_accumulate_(x), sum_base)
  expect_equal(sum_dbl_sexp_for_(x), sum_base)
})

local({
  len <- 1e5L
  expect_equal(grow_(len), as.numeric(seq(0, len - 1)))
})
