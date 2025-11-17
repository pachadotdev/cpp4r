test_that("cpp4r::stop basic behavior", {
  expect_error(my_stop_n1("This is a stop"), "This is a stop", fixed = TRUE)
})
test_that("cpp4r::warning basic behavior", {
  expect_warning(my_warning_n1("This is a warning"), "This is a warning", fixed = TRUE)
})
test_that("cpp4r::message basic behavior", {
  expect_message(my_message_n1("This is a message"), "This is a message", fixed = TRUE)
})
test_that("cpp4r::stop works without including the fmt library", {
  test1 <- "error"
  expect_error(my_stop_n1fmt("This is a stop"), "This is a stop", fixed = TRUE)
  expect_error(my_stop_n2fmt("This is an %s", test1), "This is an error", fixed = TRUE)
})
test_that("cpp4r::warning works without including the fmt library", {
  test1 <- "warning"
  expect_warning(my_warning_n1fmt("This is a warning"), "This is a warning", fixed = TRUE)
  expect_warning(my_warning_n2fmt("This is a %s", test1), "This is a warning", fixed = TRUE)
})
test_that("cpp4r::message works without including the fmt library", {
  test1 <- "message"
  expect_message(my_message_n1fmt("This is a message"), "This is a message", fixed = TRUE)
  expect_message(my_message_n2fmt("This is a %s", test1), "This is a message", fixed = TRUE)

  test2 <- c("great", "super")
  expect_message(my_message_n2fmt("You're %s", test2[2]), "You're super", fixed = TRUE)
})
