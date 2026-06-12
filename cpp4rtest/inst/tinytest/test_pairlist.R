# Tests for pairlist

expect_equal(pairlist_size_(NULL), 0L)

local({
  pl <- as.pairlist(list(a = 1L, b = 2.5))
  expect_equal(pairlist_size_(pl), 2L)
})

local({
  pl <- as.pairlist(list(a = 1L, b = 2.5))
  result <- pairlist_to_list_(pl)
  expect_equal(typeof(result), "list")
  expect_equal(names(result), c("a", "b"))
  expect_equal(result$a, 1L)
  expect_equal(result$b, 2.5)
})

expect_error(pairlist_rejects_vec_())
