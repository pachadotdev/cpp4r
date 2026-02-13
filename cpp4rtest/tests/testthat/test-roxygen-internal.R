# Tests for internal (non-exported) roxygen functions

# roxygen1.h tests (internal function)
test_that("notroxcpp1_ works", {
  expect_equal(notroxcpp1_(1.0), 2.0)
  expect_equal(notroxcpp1_(0.0), 1.0)
  expect_equal(notroxcpp1_(-1.0), 0.0)
})

# roxygen3.h tests (internal function)
test_that("notroxcpp6_ works", {
  expect_equal(notroxcpp6_(1.0), 7.0)
  expect_equal(notroxcpp6_(0.0), 6.0)
  expect_equal(notroxcpp6_(-1.0), 5.0)
})
