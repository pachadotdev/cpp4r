# Tests for named list operations

local({
  res1 <- named_list_push_back_()
  res2 <- named_list_c_style_()

  tinytest::expect_equal(res1, res2)

  tinytest::expect_equal(res1[[1]], 1.0)
  tinytest::expect_equal(res1[[2]], 2.0)
  tinytest::expect_equal(res1[[3]], 3.0)
  tinytest::expect_equal(res1[[4]], 4L)
  tinytest::expect_equal(res1[[5]], FALSE)

  tinytest::expect_equal(res2[["six"]], "six")
  tinytest::expect_equal(res2[["seven"]], "seven")
  tinytest::expect_equal(res2[["eight"]], rep(8L, 3L))
  tinytest::expect_equal(res2[["nine"]], rep(9.0, 3L))
  tinytest::expect_equal(res2[["ten"]], rep("ten", 2L))
})
