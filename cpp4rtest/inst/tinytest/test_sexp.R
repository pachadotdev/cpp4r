# Tests for sexp list initialization

local({
  result <- sexp_list_init_()
  tinytest::expect_inherits(result, "data.frame")
  tinytest::expect_equal(nrow(result), 3)
  tinytest::expect_equal(names(result), c("int", "dbl", "char"))
})

local({
  result <- sexp_scalar_list_init_()
  tinytest::expect_inherits(result, "data.frame")
  tinytest::expect_equal(nrow(result), 1)
  tinytest::expect_equal(names(result), c("int", "dbl", "char"))
})
