# Tests for complex number operations  

test_that("complex number creation works", {
  real <- c(1.0, 2.0, 3.0)
  imag <- c(4.0, 5.0, 6.0)
  
  result <- cpp4r_make_complex_(real, imag)
  expect_type(result, "complex")
  expect_length(result, 3)
  
  expect_equal(Re(result), real)
  expect_equal(Im(result), imag)
})

test_that("complex real extraction works", {
  x <- complex(real = c(1, 2, 3), imaginary = c(4, 5, 6))
  result <- cpp4r_complex_real_(x)
  expect_equal(as.numeric(result), c(1, 2, 3))
})

test_that("complex imaginary extraction works", {
  x <- complex(real = c(1, 2, 3), imaginary = c(4, 5, 6))
  result <- cpp4r_complex_imag_(x)
  expect_equal(as.numeric(result), c(4, 5, 6))
})

test_that("complex modulus works", {
  x <- complex(real = c(3, 0), imaginary = c(4, 5))
  result <- cpp4r_complex_modulus_(x)
  expect_equal(as.numeric(result), c(5, 5))  # sqrt(3^2 + 4^2) = 5, sqrt(0^2 + 5^2) = 5
})

test_that("complex addition works", {
  x <- complex(real = c(1, 2), imaginary = c(3, 4))
  y <- complex(real = c(5, 6), imaginary = c(7, 8))
  
  result <- cpp4r_complex_add_(x, y)
  expected <- x + y
  
  expect_equal(Re(result), Re(expected))
  expect_equal(Im(result), Im(expected))
})

test_that("complex empty vector", {
  x <- complex(0)
  result <- cpp4r_complex_real_(x)
  expect_length(result, 0)
})
