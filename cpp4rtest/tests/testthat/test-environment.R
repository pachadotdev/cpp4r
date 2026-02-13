# Tests for environment operations

test_that("environment get works", {
  e <- new.env()
  e$x <- 42L
  e$y <- "hello"
  e$z <- c(1.0, 2.0, 3.0)

  result <- cpp4r_env_get_int_(e, "x")
  expect_equal(result, 42L)

  result <- cpp4r_env_get_str_(e, "y")
  expect_equal(result, "hello")
})

test_that("environment set works", {
  e <- new.env()
  cpp4r_env_set_(e, "test", 123L)
  expect_equal(e$test, 123L)
})

test_that("environment exists works", {
  e <- new.env()
  e$exists <- TRUE

  expect_true(cpp4r_env_exists_(e, "exists"))
  expect_false(cpp4r_env_exists_(e, "not_exists"))
})

test_that("global environment access", {
  .GlobalEnv$cpp4r_test_var <- 999L
  on.exit(rm(cpp4r_test_var, envir = .GlobalEnv))

  result <- cpp4r_global_get_("cpp4r_test_var")
  expect_equal(result, 999L)
})

test_that("parent environment works", {
  parent <- new.env()
  parent$parent_var <- "from_parent"

  child <- new.env(parent = parent)
  child$child_var <- "from_child"

  # Should be able to find parent_var through inheritance
  expect_true(cpp4r_env_exists_(child, "child_var"))
})
