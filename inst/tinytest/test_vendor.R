# Tests for vendor() and unvendor()

source(system.file("tinytest", "helper.R", package = "cpp4r"))

# vendor errors if cpp4r is already vendored
local({
  if (!requireNamespace("desc")) { return(NULL) }
  pkg <- local_package()
  vendor(pkg)
  expect_error(
    vendor(pkg),
    pattern = "already exists"
  )
})

# vendor to non-default directory
local({
  if (!requireNamespace("desc")) { return(NULL) }
  pkg <- local_package()
  p <- paste(pkg, "inst", "include", sep = "/")
  vendor(p)
  expect_true(dir.exists(file.path(p, "cpp4r")))
  expect_true(file.exists(file.path(p, "cpp4r.hpp")))
  expect_true(file.exists(file.path(p, "cpp4r", "declarations.hpp")))
  unvendor(p)
})

# unvendor without errors
local({
  if (!requireNamespace("desc")) { return(NULL) }
  pkg <- local_package()
  p <- paste(pkg, "inst", "include", sep = "/")
  vendor(p)
  expect_true(dir.exists(file.path(p, "cpp4r")))
  expect_true(file.exists(file.path(p, "cpp4r.hpp")))
  expect_true(file.exists(file.path(p, "cpp4r", "declarations.hpp")))
  unvendor(p)
  expect_false(dir.exists(file.path(p, "cpp4r")))
  expect_false(file.exists(file.path(p, "cpp4r.hpp")))
  expect_false(file.exists(file.path(p, "cpp4r", "declarations.hpp")))
})
