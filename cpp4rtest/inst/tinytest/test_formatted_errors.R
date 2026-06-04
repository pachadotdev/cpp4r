# Tests for formatted errors

tinytest::expect_error(my_stop_n1("This is a stop"), pattern = "This is a stop")
tinytest::expect_warning(my_warning_n1("This is a warning"), pattern = "This is a warning")
tinytest::expect_message(my_message_n1("This is a message"), pattern = "This is a message")

local({
  test1 <- "error"
  tinytest::expect_error(my_stop_n1fmt("This is a stop"), pattern = "This is a stop")
  tinytest::expect_error(my_stop_n2fmt("This is an %s", test1), pattern = "This is an error")
})

local({
  test1 <- "warning"
  tinytest::expect_warning(my_warning_n1fmt("This is a warning"), pattern = "This is a warning")
  tinytest::expect_warning(my_warning_n2fmt("This is a %s", test1), pattern = "This is a warning")
})

local({
  test1 <- "message"
  tinytest::expect_message(my_message_n1fmt("This is a message"), pattern = "This is a message")
  tinytest::expect_message(my_message_n2fmt("This is a %s", test1), pattern = "This is a message")

  test2 <- c("great", "super")
  tinytest::expect_message(my_message_n2fmt("You're %s", test2[2]), pattern = "You're super")
})
