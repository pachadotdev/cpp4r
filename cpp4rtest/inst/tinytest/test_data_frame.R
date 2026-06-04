# Tests for data_frame.h functions

local({
  result <- data_frame_()
  tinytest::expect_inherits(result, "data.frame")
  tinytest::expect_equal(nrow(result), 3)
  tinytest::expect_equal(ncol(result), 2)
  tinytest::expect_equal(result$nums, c(1, 2, 3))
  tinytest::expect_equal(result$letters, c("x", "y", "z"))
})
