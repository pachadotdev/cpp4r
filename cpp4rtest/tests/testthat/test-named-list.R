test_that("creating named lists with push_back or C-style gives the same result", {
  res1 <- cpp4r_named_list_push_back_()
  res2 <- cpp4r_named_list_c_style_()

  expect_equal(res1, res2)

  expect_equal(res1[[1]], 1.0)
  expect_equal(res1[[2]], 2.0)
  expect_equal(res1[[3]], 3.0)
  expect_equal(res1[[4]], 4L)
  expect_equal(res1[[5]], FALSE)

  expect_equal(res2[["six"]], "six")
  expect_equal(res2[["seven"]], "seven")
  expect_equal(res2[["eight"]], rep(8L, 3L))
  expect_equal(res2[["nine"]], rep(9.0, 3L))
  expect_equal(res2[["ten"]], rep("ten", 2L))
})
