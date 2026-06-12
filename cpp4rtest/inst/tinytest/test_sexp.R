# Tests for sexp list initialization

local({
  result <- sexp_list_init_()
  expect_inherits(result, "data.frame")
  expect_equal(nrow(result), 3)
  expect_equal(names(result), c("int", "dbl", "char"))
})

local({
  result <- sexp_scalar_list_init_()
  expect_inherits(result, "data.frame")
  expect_equal(nrow(result), 1)
  expect_equal(names(result), c("int", "dbl", "char"))
})
