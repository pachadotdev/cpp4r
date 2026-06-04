# Tests for add.h functions

x <- c(1.0, 2.0, 3.0)
result <- add_vec_for_(x, 10.0)
tinytest::expect_equal(as.numeric(result), c(11.0, 12.0, 13.0))

x <- numeric(0)
result <- add_vec_for_(x, 5.0)
tinytest::expect_equal(length(result), 0)

x <- c(-1.0, 0.0, 1.0)
result <- add_vec_for_(x, -5.0)
tinytest::expect_equal(as.numeric(result), c(-6.0, -5.0, -4.0))
