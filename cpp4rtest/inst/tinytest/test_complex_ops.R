# Tests for complex number operations

local({
  real <- c(1.0, 2.0, 3.0)
  imag <- c(4.0, 5.0, 6.0)
  result <- make_complex_(real, imag)
  expect_equal(typeof(result), "complex")
  expect_equal(length(result), 3)
  expect_equal(Re(result), real)
  expect_equal(Im(result), imag)
})

local({
  x <- complex(real = c(1, 2, 3), imaginary = c(4, 5, 6))
  result <- complex_real_(x)
  expect_equal(as.numeric(result), c(1, 2, 3))
})

local({
  x <- complex(real = c(1, 2, 3), imaginary = c(4, 5, 6))
  result <- complex_imag_(x)
  expect_equal(as.numeric(result), c(4, 5, 6))
})

local({
  x <- complex(real = c(3, 0), imaginary = c(4, 5))
  result <- complex_modulus_(x)
  expect_equal(as.numeric(result), c(5, 5))
})

local({
  x <- complex(real = c(1, 2), imaginary = c(3, 4))
  y <- complex(real = c(5, 6), imaginary = c(7, 8))
  result <- complex_add_(x, y)
  expected <- x + y
  expect_equal(Re(result), Re(expected))
  expect_equal(Im(result), Im(expected))
})

local({
  x <- complex(0)
  result <- complex_real_(x)
  expect_equal(length(result), 0)
})
