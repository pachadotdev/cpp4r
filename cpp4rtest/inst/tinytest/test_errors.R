# Tests for formatted errors

expect_error(my_stop_n1_("This is a stop"), pattern = "This is a stop")
expect_warning(my_warning_n1_("This is a warning"), pattern = "This is a warning")
expect_message(my_message_n1_("This is a message"), pattern = "This is a message")

local({
  test1 <- "error"
  expect_error(my_stop_n1_("This is a stop"), pattern = "This is a stop")
  expect_error(my_stop_n2_("This is an %s", test1), pattern = "This is an error")
})

local({
  test1 <- "warning"
  expect_warning(my_warning_n1_("This is a warning"), pattern = "This is a warning")
  expect_warning(my_warning_n2_("This is a %s", test1), pattern = "This is a warning")
})

local({
  test1 <- "message"
  expect_message(my_message_n1_("This is a message"), pattern = "This is a message")
  expect_message(my_message_n2_("This is a %s", test1), pattern = "This is a message")

  test2 <- c("great", "super")
  expect_message(my_message_n2_("You're %s", test2[2]), pattern = "You're super")
})
