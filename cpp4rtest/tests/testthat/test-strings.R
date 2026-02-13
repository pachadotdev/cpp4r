# Tests for strings.h functions

test_that("grow_strings_cpp4r_ works", {
  result <- grow_strings_cpp4r_(5L, 123L)
  expect_length(result, 5)
  expect_type(result, "character")
})

test_that("grow_strings_manual_ works", {
  result <- grow_strings_manual_(5L, 123L)
  expect_length(result, 5)
  expect_type(result, "character")
})

test_that("assign_cpp4r_ works", {
  result <- assign_cpp4r_(5L, 123L)
  expect_length(result, 5)
  expect_type(result, "character")
})

test_that("grow functions produce consistent results with same seed", {
  res1 <- grow_strings_cpp4r_(10L, 42L)
  res2 <- grow_strings_manual_(10L, 42L)
  res3 <- assign_cpp4r_(10L, 42L)

  expect_equal(res1, res2)
  expect_equal(res1, res3)
})
