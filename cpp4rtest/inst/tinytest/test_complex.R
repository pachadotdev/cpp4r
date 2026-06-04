# Tests for complex iterators

local({
  len <- 1e5
  set.seed(42)
  x <- complex(real = rnorm(len), imaginary = rnorm(len))
  sum_base <- sum(x)

  expect_equal(sum_cplx_r_complex_out_(x), sum_base)
  expect_equal(sum_cplx_complexes_out_(x), sum_base)
  expect_equal(sum_cplx_typed_std_out_(x), sum_base)
  expect_equal(sum_cplx_sexp_std_out_(x), sum_base)
  expect_equal(sum_cplx_sexp_sexp_out_(x), sum_base)
  expect_equal(sum_cplx_sexp_complexes_out_(x), sum_base)

  expect_equal(sum_cplx_foreach_(x), sum_base)
  expect_equal(sum_cplx_accumulate_(x), sum_base)
  expect_equal(sum_cplx_sexp_protect_std_out_(x), sum_base)
})

local({
  len <- 1e5
  seq_complex <- function(x) complex(real = as.double(seq_len(x)), imaginary = as.double(seq_len(x)))
  x <- seq_complex(len)
  sum_base <- sum(x)

  expect_equal(sum_cplx_r_complex_out_(x), sum_base)
  expect_equal(sum_cplx_foreach_(x), sum_base)
  expect_equal(sum_cplx_accumulate_(x), sum_base)
  expect_equal(sum_cplx_sexp_protect_std_out_(x), sum_base)
})

local({
  len <- 1e5L
  expect_equal(grow_cplx_(len), complex(
    real = as.numeric(seq(0, len - 1)),
    imaginary = as.numeric(seq(0, len - 1))
  ))
})
