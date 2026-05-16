# Tests for protect.h and release.h functions
# These test memory protection mechanisms - we verify they don't crash
# and that the protected objects remain valid

# protect.h tests - single object protection
test_that("protect_one_rapi_ completes without error", {
  x <- c(1.0, 2.0, 3.0)
  # Run protection cycle and verify object is still valid after

  protect_one_rapi_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

test_that("protect_one_sexp_ completes without error", {
  x <- c(1.0, 2.0, 3.0)
  protect_one_sexp_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

test_that("protect_one_ completes without error", {
  x <- c(1.0, 2.0, 3.0)
  protect_one_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

test_that("protect_one_preserve_ completes without error", {
  x <- c(1.0, 2.0, 3.0)
  protect_one_preserve_(x, 10L)
  expect_equal(x, c(1.0, 2.0, 3.0))
})

# protect.h tests - multiple object protection
test_that("protect_many_rapi_ completes without error", {
  # This creates n protected integers internally

  expect_no_error(protect_many_rapi_(10L))
})

test_that("protect_many_ completes without error", {
  expect_no_error(protect_many_(10L))
})

test_that("protect_many_sexp_ completes without error", {
  expect_no_error(protect_many_sexp_(10L))
})

test_that("protect_many_preserve_ completes without error", {
  expect_no_error(protect_many_preserve_(10L))
})

# release.h tests
test_that("release_ completes without error", {
  # This creates and releases n sexp objects
  expect_no_error(release_(10L))
})

test_that("release_ handles larger allocations", {
  expect_no_error(release_(100L))
})
