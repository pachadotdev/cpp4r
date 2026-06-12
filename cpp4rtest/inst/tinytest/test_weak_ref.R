# Tests for weak_ref

local({
  key <- new.env()
  val <- list(1, 2, 3)
  expect_true(weak_ref_make_alive_(key, val))
})

local({
  expect_false(weak_ref_nil_not_alive_())
})

local({
  key <- new.env()
  val <- list(42)
  result <- weak_ref_value_(key, val)
  expect_equal(result, list(42))
})

local({
  key <- new.env()
  val <- list(1)
  expect_equal(weak_ref_typeof_(key, val), 23L) # WEAKREFSXP = 23
})

expect_error(weak_ref_rejects_vec_())
