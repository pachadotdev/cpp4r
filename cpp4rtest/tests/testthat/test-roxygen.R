test_that("roxygen headers correctly register functions", {
  expect_equal(notroxcpp1(1.0), 2.0)
  expect_equal(roxcpp2(1.0), 3.0)
  expect_equal(roxcpp3(1.0), 4.0)
  expect_equal(roxcpp4(1.0), 5.0)
  expect_equal(roxcpp5(1.0), 6.0)
  expect_equal(notroxcpp6(1.0), 7.0)
  expect_equal(roxcpp7(1.0), 8.0)
})
