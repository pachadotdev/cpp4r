# Tests for ordered and unordered map to list

local({
  set.seed(42L)
  x <- rnorm(10L)
  xprime <- c(x, x[1])

  om <- ordered_map_to_list_(x)
  expect_equal(typeof(om), "list")

  om_doubles <- as.double(names(om))
  expect_equal(om_doubles, sort(om_doubles))

  omprime <- ordered_map_to_list_(xprime)
  expect_equal(unlist(unique(omprime)), 1:2)

  um <- unordered_map_to_list_(xprime)
  expect_equal(typeof(um), "list")
  expect_equal(unlist(unique(um)), 1:2)
})
