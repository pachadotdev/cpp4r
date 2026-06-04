# Tests for errors.h and errors_fmt.h functions

tinytest::expect_error(my_stop("test error", 1L), pattern = "test error")
tinytest::expect_warning(my_warning("test warning", "arg"), pattern = "test warning")
tinytest::expect_message(my_message("test message", "arg"), pattern = "test message")
