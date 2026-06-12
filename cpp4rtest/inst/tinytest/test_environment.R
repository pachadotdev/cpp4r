# Tests for environment operations

local({
  e <- new.env()
  e$x <- 42L
  e$y <- "hello"

  result <- env_get_int_(e, "x")
  expect_equal(result, 42L)

  result <- env_get_str_(e, "y")
  expect_equal(result, "hello")
})

local({
  e <- new.env()
  env_set_(e, "test", 123L)
  expect_equal(e$test, 123L)
})

local({
  e <- new.env()
  e$exists <- TRUE
  expect_true(env_exists_(e, "exists"))
  expect_false(env_exists_(e, "not_exists"))
})

local({
  .GlobalEnv$test_var <- 999L
  on.exit(rm(test_var, envir = .GlobalEnv))
  result <- global_get_("test_var")
  expect_equal(result, 999L)
})

local({
  parent <- new.env()
  parent$parent_var <- "from_parent"
  child <- new.env(parent = parent)
  child$child_var <- "from_child"
  expect_true(env_exists_(child, "child_var"))
})
