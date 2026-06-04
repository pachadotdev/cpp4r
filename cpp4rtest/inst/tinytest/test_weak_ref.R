# Tests for weak_ref

local({
  key <- new.env()
  val <- list(1, 2, 3)
  tinytest::expect_true(weak_ref_make_alive_(key, val))
})

local({
  tinytest::expect_false(weak_ref_nil_not_alive_())
})

local({
  key <- new.env()
  val <- list(42)
  result <- weak_ref_value_(key, val)
  tinytest::expect_equal(result, list(42))
})

local({
  key <- new.env()
  val <- list(1)
  tinytest::expect_equal(weak_ref_typeof_(key, val), 26L) # WEAKREFSXP = 26
})

tinytest::expect_error(weak_ref_rejects_vec_())
