# Tests for errors.h and errors_fmt.h functions

# errors_fmt.h tests
test_that("my_stop with extra arg works", {
  expect_error(my_stop("test error", 1L), "test error")
})

test_that("my_warning with extra arg works", {
  expect_warning(my_warning("test warning", "arg"), "test warning")
})

test_that("my_message with extra arg works", {
  expect_message(my_message("test message", "arg"), "test message")
})
