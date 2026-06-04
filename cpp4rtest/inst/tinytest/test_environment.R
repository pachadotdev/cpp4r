# Tests for environment operations

local({
  e <- new.env()
  e$x <- 42L
  e$y <- "hello"

  result <- env_get_int_(e, "x")
  tinytest::expect_equal(result, 42L)

  result <- env_get_str_(e, "y")
  tinytest::expect_equal(result, "hello")
})

local({
  e <- new.env()
  env_set_(e, "test", 123L)
  tinytest::expect_equal(e$test, 123L)
})

local({
  e <- new.env()
  e$exists <- TRUE
  tinytest::expect_true(env_exists_(e, "exists"))
  tinytest::expect_false(env_exists_(e, "not_exists"))
})

local({
  .GlobalEnv$test_var <- 999L
  on.exit(rm(test_var, envir = .GlobalEnv))
  result <- global_get_("test_var")
  tinytest::expect_equal(result, 999L)
})

local({
  parent <- new.env()
  parent$parent_var <- "from_parent"
  child <- new.env(parent = parent)
  child$child_var <- "from_child"
  tinytest::expect_true(env_exists_(child, "child_var"))
})
