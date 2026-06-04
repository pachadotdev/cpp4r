# Tests for raw byte operations

local({
  x <- as.raw(c(0, 1, 2, 255))
  expect_equal(raw_copy_(x), x)
})

local({
  x <- raw(0)
  expect_equal(length(raw_copy_(x)), 0)
})

local({
  x <- as.raw(c(0, 128, 255))
  expect_equal(raw_xor_(x, as.raw(0xFF)), as.raw(c(255, 127, 0)))
})
