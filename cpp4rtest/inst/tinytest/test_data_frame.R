# Tests for data_frame.h functions

local({
  result <- data_frame_()
  expect_inherits(result, "data.frame")
  expect_equal(nrow(result), 3)
  expect_equal(ncol(result), 2)
  expect_equal(result$nums, c(1, 2, 3))
  expect_equal(result$letters, c("x", "y", "z"))
})
