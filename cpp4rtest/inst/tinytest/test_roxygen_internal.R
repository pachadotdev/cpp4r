# Tests for internal (non-exported) roxygen functions

tinytest::expect_equal(notroxcpp1_(1.0), 2.0)
tinytest::expect_equal(notroxcpp1_(0.0), 1.0)
tinytest::expect_equal(notroxcpp1_(-1.0), 0.0)

tinytest::expect_equal(notroxcpp6_(1.0), 7.0)
tinytest::expect_equal(notroxcpp6_(0.0), 6.0)
tinytest::expect_equal(notroxcpp6_(-1.0), 5.0)
