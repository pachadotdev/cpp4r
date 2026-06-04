# Tests for strings.h functions

local({
  result <- grow_strings_(5L, 123L)
  expect_equal(length(result), 5)
  expect_equal(typeof(result), "character")
})

local({
  result <- grow_strings_manual_(5L, 123L)
  expect_equal(length(result), 5)
  expect_equal(typeof(result), "character")
})

local({
  result <- assign_(5L, 123L)
  expect_equal(length(result), 5)
  expect_equal(typeof(result), "character")
})

local({
  res1 <- grow_strings_(10L, 42L)
  res2 <- grow_strings_manual_(10L, 42L)
  res3 <- assign_(10L, 42L)
  expect_equal(res1, res2)
  expect_equal(res1, res3)
})
