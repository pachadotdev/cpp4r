# Tests for register-related functions
# This tests the package template function indirectly

source(system.file("tinytest", "helper.R", package = "cpp4r"))

# --- get_call_entries ---

# minimal table for no registered functions
local({
  pkg <- local_package()
  path <- pkg_path(pkg)
  expect_equal(
    cpp4r:::get_call_entries(path, get_funs(path), get_package_name(path)),
    c(
      "static const R_CallMethodDef CallEntries[] = {",
      "    {NULL, NULL, 0}",
      "};"
    )
  )
})

# works with multiple register functions
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("multiple.cpp"), file.path(p, "src", "multiple.cpp"))
  cpp4r::register(p)
  cpp_bindings <- file.path(p, "src", "cpp4r.cpp")
  content <- read_file(cpp_bindings)
  expect_true(grepl("_testPkg_foo", content, fixed = TRUE))
  expect_true(grepl("_testPkg_bar", content, fixed = TRUE))
  expect_true(grepl("_testPkg_baz", content, fixed = TRUE))
  expect_true(grepl("R_registerRoutines", content, fixed = TRUE))
})

# --- wrap_call ---

expect_equal(
  cpp4r:::wrap_call("foo", "void", list(type = character(), name = character())),
  "  foo();\n    return R_NilValue;"
)

expect_equal(
  cpp4r:::wrap_call("foo", "bool", list(type = character(), name = character())),
  "  return cpp4r::as_sexp(foo());"
)

expect_equal(
  cpp4r:::wrap_call("foo", "void", list(type = c("double", "int"), name = c("x", "y"))),
  "  foo(cpp4r::as_cpp<cpp4r::decay_t<double>>(x), cpp4r::as_cpp<cpp4r::decay_t<int>>(y));\n    return R_NilValue;"
)

expect_equal(
  cpp4r:::wrap_call("foo", "bool", list(type = c("double", "int"), name = c("x", "y"))),
  "  return cpp4r::as_sexp(foo(cpp4r::as_cpp<cpp4r::decay_t<double>>(x), cpp4r::as_cpp<cpp4r::decay_t<int>>(y)));"
)

# --- get_registered_functions ---

# empty tibble for non-existent file
local({
  f <- tempfile()
  decorations <- cpp4r:::cpp_decorations(files = f, is_attribute = TRUE)
  res <- cpp4r:::get_registered_functions(decorations, "cpp4r::register")
  expect_equal(
    names(res),
    c("file", "line", "decoration", "namespace", "params", "context", "name", "cpp_name", "return_type", "args")
  )
  expect_equal(length(res$file), 0)
})

# empty tibble for empty file
local({
  f <- tempfile()
  file.create(f)
  decorations <- cpp4r:::cpp_decorations(files = f, is_attribute = TRUE)
  res <- cpp4r:::get_registered_functions(decorations, "cpp4r::register")
  expect_equal(length(res$file), 0)
})

# single registration
local({
  decorations <- cpp4r:::cpp_decorations(files = test_path("single.cpp"), is_attribute = TRUE)
  res <- cpp4r:::get_registered_functions(decorations, "cpp4r::register")
  expect_equal(length(res$file), 1L)
  expect_equal(res$name, "foo")
  expect_equal(res$cpp_name, "foo")
  expect_equal(res$namespace, "")
  expect_equal(res$return_type, "int")
  expect_equal(names(res$args[[1]]), c("type", "name", "default"))
  expect_equal(length(res$args[[1]]$name), 0)
})

# multiple registrations
local({
  decorations <- cpp4r:::cpp_decorations(files = test_path("multiple.cpp"), is_attribute = TRUE)
  res <- cpp4r:::get_registered_functions(decorations, "cpp4r::register")
  expect_equal(length(res$file), 3L)
  expect_equal(res$name, c("foo", "bar", "baz"))
  expect_equal(res$return_type, c("int", "double", "bool"))

  expect_equal(length(res$args[[1]]$name), 0)

  expect_equal(length(res$args[[2]]$name), 1)
  expect_equal(res$args[[2]]$type, "bool")
  expect_equal(res$args[[2]]$name, "run")
  expect_equal(res$args[[2]]$default, NA_character_)

  expect_equal(length(res$args[[3]]$name), 2)
  expect_equal(res$args[[3]]$type, c("bool", "int"))
  expect_equal(res$args[[3]]$name, c("run", "value"))
  expect_equal(res$args[[3]]$default, c(NA_character_, "0"))
})

# --- generate_cpp_functions ---

local({
  tmp <- tempfile()
  on.exit(unlink(tmp, recursive = TRUE))
  dir.create(tmp)
  writeLines("Package: mypkg", file.path(tmp, "DESCRIPTION"))
  writeLines("useDynLib(mypkg, .registration = TRUE)", file.path(tmp, "NAMESPACE"))
  tmpl_src <- system.file("extdata/pkgtemplate/src", package = "cpp4r")
  file.copy(tmpl_src, tmp, recursive = TRUE)
  register(tmp)
  cpp_content <- paste(readLines(file.path(tmp, "src", "cpp4r.cpp")), collapse = "\n")
  expect_true(grepl("_mypkg_plus_one", cpp_content, fixed = TRUE))
  expect_true(grepl("_mypkg_plus_two", cpp_content, fixed = TRUE))
  r_content <- paste(readLines(file.path(tmp, "R", "cpp4r.R")), collapse = "\n")
  expect_true(grepl("plus_one", r_content, fixed = TRUE))
  expect_true(grepl("plus_two", r_content, fixed = TRUE))
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_cpp_functions(funs),
    "// foo.cpp\nvoid foo();\nextern \"C\" SEXP _cpp4r_foo() {\n  BEGIN_CPP4R\n    foo();\n    return R_NilValue;\n  END_CPP4R\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_cpp_functions(funs, package = "mypkg"),
    "// foo.cpp\nvoid foo();\nextern \"C\" SEXP _mypkg_foo() {\n  BEGIN_CPP4R\n    foo();\n    return R_NilValue;\n  END_CPP4R\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "int",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_cpp_functions(funs),
    "// foo.cpp\nint foo();\nextern \"C\" SEXP _cpp4r_foo() {\n  BEGIN_CPP4R\n    return cpp4r::as_sexp(foo());\n  END_CPP4R\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = "int", name = "bar"))
  )
  expect_equal(
    cpp4r:::generate_cpp_functions(funs),
    "// foo.cpp\nvoid foo(int bar);\nextern \"C\" SEXP _cpp4r_foo(SEXP bar) {\n  BEGIN_CPP4R\n    foo(cpp4r::as_cpp<cpp4r::decay_t<int>>(bar));\n    return R_NilValue;\n  END_CPP4R\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "int",
    args = list(list(type = "int", name = "bar"))
  )
  expect_equal(
    cpp4r:::generate_cpp_functions(funs),
    "// foo.cpp\nint foo(int bar);\nextern \"C\" SEXP _cpp4r_foo(SEXP bar) {\n  BEGIN_CPP4R\n    return cpp4r::as_sexp(foo(cpp4r::as_cpp<cpp4r::decay_t<int>>(bar)));\n  END_CPP4R\n}"
  )
})

local({
  funs <- list(
    file = c("foo.cpp", "bar.cpp"), line = c(1L, 3L),
    decoration = c("cpp4r", "cpp4r"), params = list(NA, NA),
    context = list(NA_character_, NA_character_),
    name = c("foo", "bar"), return_type = c("int", "bool"),
    args = list(
      list(type = "int", name = "bar"),
      list(type = "double", name = "baz")
    )
  )
  expect_equal(
    cpp4r:::generate_cpp_functions(funs),
    paste0(
      "// foo.cpp\nint foo(int bar);\nextern \"C\" SEXP _cpp4r_foo(SEXP bar) {\n  BEGIN_CPP4R\n    return cpp4r::as_sexp(foo(cpp4r::as_cpp<cpp4r::decay_t<int>>(bar)));\n  END_CPP4R\n}",
      "\n",
      "// bar.cpp\nbool bar(double baz);\nextern \"C\" SEXP _cpp4r_bar(SEXP baz) {\n  BEGIN_CPP4R\n    return cpp4r::as_sexp(bar(cpp4r::as_cpp<cpp4r::decay_t<double>>(baz)));\n  END_CPP4R\n}"
    )
  )
})

# --- generate_r_functions ---

local({
  funs <- list(
    file = character(), line = integer(), decoration = character(),
    params = list(), context = list(), name = character(),
    return_type = character(), args = list()
  )
  expect_equal(cpp4r:::generate_r_functions(funs), "")
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r"),
    "foo <- function() {\n\tinvisible(.Call(`_cpp4r_foo`))\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r", use_package = TRUE),
    "foo <- function() {\n\tinvisible(.Call(\"_cpp4r_foo\", PACKAGE = \"cpp4r\"))\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "mypkg"),
    "foo <- function() {\n\tinvisible(.Call(`_mypkg_foo`))\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "int",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r"),
    "foo <- function() {\n\t.Call(`_cpp4r_foo`)\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "int",
    args = list(list(type = character(), name = character()))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r", use_package = TRUE),
    "foo <- function() {\n\t.Call(\"_cpp4r_foo\", PACKAGE = \"cpp4r\")\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = "int", name = "bar"))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r"),
    "foo <- function(bar) {\n\tinvisible(.Call(`_cpp4r_foo`, bar))\n}"
  )
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "int",
    args = list(list(type = "int", name = "bar"))
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r"),
    "foo <- function(bar) {\n\t.Call(`_cpp4r_foo`, bar)\n}"
  )
})

local({
  funs <- list(
    file = c("foo.cpp", "bar.cpp"), line = c(1L, 3L),
    decoration = c("cpp4r", "cpp4r"), params = list(NA, NA),
    context = list(NA_character_, NA_character_),
    name = c("foo", "bar"), return_type = c("int", "bool"),
    args = list(
      list(type = "int", name = "bar"),
      list(type = "double", name = "baz")
    )
  )
  expect_equal(
    cpp4r:::generate_r_functions(funs, package = "cpp4r"),
    "foo <- function(bar) {\n\t.Call(`_cpp4r_foo`, bar)\n}\n\nbar <- function(baz) {\n\t.Call(`_cpp4r_bar`, baz)\n}"
  )
})

# --- register ---

local({
  f <- tempdir()
  expect_equal(cpp4r::register(f), character())
  dir.create(f, showWarnings = FALSE)
  expect_equal(cpp4r::register(f), character())
})

# register with single C++ function
local({
  exit_if_not(getRversion() >= "3.4")
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  cpp4r::register(p)
  r_bindings <- file.path(p, "R", "cpp4r.R")
  expect_true(file.exists(r_bindings))
  r_content <- read_file(r_bindings)
  expect_true(grepl("_testPkg_foo", r_content, fixed = TRUE))
  cpp_bindings <- file.path(p, "src", "cpp4r.cpp")
  expect_true(file.exists(cpp_bindings))
  cpp_content <- read_file(cpp_bindings)
  expect_true(grepl("_testPkg_foo", cpp_content, fixed = TRUE))
  expect_true(grepl("R_registerRoutines", cpp_content, fixed = TRUE))
})

# can be run without messages
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  expect_silent(cpp4r::register(p, quiet = TRUE))
})

# can be run with messages
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  msgs <- character(0)
  withCallingHandlers(
    cpp4r::register(p, quiet = FALSE),
    message = function(m) {
      msgs <<- c(msgs, conditionMessage(m))
      invokeRestart("muffleMessage")
    }
  )
  expect_true(any(grepl("cpp4r::register", msgs)))
})

# includes pkg_types.h if in src
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  writeLines("#include <sstream>", file.path(p, "src", "testPkg_types.h"))
  register(p)
  expect_true(any(grepl(
    '#include "testPkg_types.h"',
    readLines(file.path(p, "src", "cpp4r.cpp")),
    fixed = TRUE
  )))
})

# includes pkg_types.hpp if in src
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  writeLines("#include <sstream>", file.path(p, "src", "testPkg_types.hpp"))
  register(p)
  expect_true(any(grepl(
    '#include "testPkg_types.hpp"',
    readLines(file.path(p, "src", "cpp4r.cpp")),
    fixed = TRUE
  )))
})

# includes pkg_types.h if in inst/include
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  dir.create(file.path(p, "inst", "include"), recursive = TRUE)
  writeLines("#include <sstream>", file.path(p, "inst", "include", "testPkg_types.h"))
  register(p)
  expect_true(any(grepl(
    '#include "testPkg_types.h"',
    readLines(file.path(p, "src", "cpp4r.cpp")),
    fixed = TRUE
  )))
})

# includes pkg_types.hpp if in inst/include
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  dir.create(file.path(p, "inst", "include"), recursive = TRUE)
  writeLines("#include <sstream>", file.path(p, "inst", "include", "testPkg_types.hpp"))
  register(p)
  expect_true(any(grepl(
    '#include "testPkg_types.hpp"',
    readLines(file.path(p, "src", "cpp4r.cpp")),
    fixed = TRUE
  )))
})

# does not error if no files have registered functions
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  writeLines("int foo(int x) { return x; }", file.path(p, "src", "foo.cpp"))
  result <- tryCatch(register(p), error = function(e) e)
  expect_false(inherits(result, "error"))
})

# accepts .cc extension
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cc"))
  register(p, extension = ".cc")
  expect_true(any(grepl("\\.cc$", list.files(file.path(p, "src")))))
})

# --- generate_init_functions ---

local({
  funs <- list(
    file = character(), line = integer(), decoration = character(),
    params = list(), context = list(), name = character(),
    return_type = character(), args = list()
  )
  expect_equal(cpp4r:::generate_init_functions(funs), list(declarations = "", calls = ""))
})

local({
  funs <- list(
    file = "foo.cpp", line = 1L, decoration = "cpp4r",
    params = list(NA), context = list(NA_character_),
    name = "foo", return_type = "void",
    args = list(list(type = "DllInfo*", name = "dll"))
  )
  expect_equal(
    cpp4r:::generate_init_functions(funs),
    list(declarations = "\nvoid foo(DllInfo* dll);\n", calls = "\n  foo(dll);")
  )
})

local({
  funs <- list(
    file = c("foo.cpp", "bar.cpp"), line = c(1L, 3L),
    decoration = c("cpp4r", "cpp4r"), params = list(NA, NA),
    context = list(NA_character_, NA_character_),
    name = c("foo", "bar"), return_type = c("void", "void"),
    args = list(
      list(type = "DllInfo*", name = "dll"),
      list(type = "DllInfo*", name = "dll")
    )
  )
  expect_equal(
    cpp4r:::generate_init_functions(funs),
    list(
      declarations = "\nvoid foo(DllInfo* dll);\nvoid bar(DllInfo* dll);\n",
      calls = "\n  foo(dll);\n  bar(dll);"
    )
  )
})

# --- check_valid_attributes ---

# no error if all registers are correct
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("single.cpp"), file.path(p, "src", "single.cpp"))
  result <- tryCatch(register(p), error = function(e) e)
  expect_false(inherits(result, "error"))
})

local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(test_path("multiple.cpp"), file.path(p, "src", "multiple.cpp"))
  result <- tryCatch(register(p), error = function(e) e)
  expect_false(inherits(result, "error"))
})

# error if one or more registers is incorrect
local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(
    test_path("single_incorrect.cpp"),
    file.path(p, "src", "single_incorrect.cpp")
  )
  expect_error(register(p))
})

local({
  pkg <- local_package()
  p <- pkg_path(pkg)
  dir.create(file.path(p, "src"))
  file.copy(
    test_path("multiple_incorrect.cpp"),
    file.path(p, "src", "multiple_incorrect.cpp")
  )
  expect_error(register(p))
})
