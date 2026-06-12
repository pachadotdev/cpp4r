# Tests for decoration parsing functions

source(system.file("tinytest", "helper.R", package = "cpp4r"))

# --- cpp_files ---

expect_equal(cpp4r:::cpp_files(""), character())

local({
  tmp <- tempfile()
  dir.create(tmp)
  on.exit(unlink(tmp, recursive = TRUE))
  expect_equal(cpp4r:::cpp_files(tmp), character())
})

local({
  tmp <- tempfile()
  dir.create(file.path(tmp, "src"), recursive = TRUE)
  on.exit(unlink(tmp, recursive = TRUE))
  file.create(file.path(tmp, "src", "foo.cpp"))
  file.create(file.path(tmp, "src", "bar.cc"))
  file.create(file.path(tmp, "src", "baz.R"))
  files <- cpp4r:::cpp_files(tmp)
  expect_true(all(grepl("[.](cpp|cc|h|hpp)$", files)))
  expect_false(any(grepl("[.]R$", files)))
  expect_equal(length(files), 2L)
})

local({
  tmp <- tempfile()
  dir.create(file.path(tmp, "src"), recursive = TRUE)
  on.exit(unlink(tmp, recursive = TRUE))
  file.create(file.path(tmp, "src", "z.cpp"))
  file.create(file.path(tmp, "src", "a.cpp"))
  files <- cpp4r:::cpp_files(tmp)
  expect_equal(basename(files), c("a.cpp", "z.cpp"))
})

# --- blank_comments ---

expect_equal(cpp4r:::blank_comments("int x = 1;"), "int x = 1;")

local({
  out <- cpp4r:::blank_comments("int x = 1; // comment\nint y = 2;")
  expect_false(grepl("comment", out))
  expect_true(grepl("int y = 2;", out))
})

local({
  out <- cpp4r:::blank_comments("/* secret\nstuff */ int x;")
  expect_false(grepl("secret", out))
  expect_true(grepl("int x;", out))
})

local({
  out <- cpp4r:::blank_comments("/* line1\nline2 */")
  expect_equal(lengths(regmatches(out, gregexpr("\n", out))), 1L)
})

local({
  code <- 'const char* s = "// not a comment";'
  out <- cpp4r:::blank_comments(code)
  expect_true(grepl("not a comment", out))
})

# --- namespace_per_line ---

local({
  lines <- c("int foo() {", "  return 1;", "}")
  expect_equal(cpp4r:::namespace_per_line(lines), c("", "", ""))
})

local({
  lines <- c("namespace myns {", "int foo() { return 1; }", "}")
  result <- cpp4r:::namespace_per_line(lines)
  expect_equal(result[[1L]], "")
  expect_equal(result[[2L]], "myns")
})

local({
  lines <- c(
    "namespace outer {",
    "namespace inner {",
    "int foo();",
    "}",
    "}"
  )
  result <- cpp4r:::namespace_per_line(lines)
  expect_equal(result[[3L]], "outer::inner")
  expect_equal(result[[5L]], "")
})

expect_equal(cpp4r:::namespace_per_line(character()), character())

# --- cpp_attribute_pattern ---

local({
  pat <- cpp4r:::cpp_attribute_pattern(is_attribute = TRUE)
  expect_true(grepl(pat, "[[cpp4r::register]]"))
})

local({
  pat <- cpp4r:::cpp_attribute_pattern(is_attribute = FALSE)
  expect_true(grepl(pat, "// [[cpp4r::register]]"))
})

local({
  pat <- cpp4r:::cpp_attribute_pattern(is_attribute = TRUE)
  expect_false(grepl(pat, "int foo() { return 1; }"))
})

# --- cpp_decorations ---

local({
  out <- cpp4r:::cpp_decorations(files = character())
  expect_equal(length(out$file), 0L)
  expect_equal(names(out), c("file", "line", "decoration", "namespace", "params", "context"))
})

local({
  out <- cpp4r:::cpp_decorations(files = tempfile())
  expect_equal(length(out$file), 0L)
})

local({
  f <- tempfile(fileext = ".cpp")
  file.create(f)
  on.exit(unlink(f))
  out <- cpp4r:::cpp_decorations(files = f, is_attribute = TRUE)
  expect_equal(length(out$file), 0L)
})

local({
  f <- tempfile(fileext = ".cpp")
  on.exit(unlink(f))
  writeLines("[[cpp4r::register]] int foo() { return 1; }", f)
  out <- cpp4r:::cpp_decorations(files = f, is_attribute = TRUE)
  expect_equal(length(out$file), 1L)
  expect_equal(out$decoration, "cpp4r::register")
  expect_equal(out$namespace, "")
})

local({
  out <- cpp4r:::cpp_decorations(files = test_path("multiple.cpp"), is_attribute = TRUE)
  expect_equal(length(out$file), 3L)
  expect_equal(out$decoration, rep("cpp4r::register", 3L))
})

local({
  f <- tempfile(fileext = ".cpp")
  on.exit(unlink(f))
  writeLines(c(
    "namespace myns {",
    "[[cpp4r::register]]",
    "int fun(int x) { return x; }",
    "}"
  ), f)
  out <- cpp4r:::cpp_decorations(files = f, is_attribute = TRUE)
  expect_equal(length(out$file), 1L)
  expect_equal(out$namespace, "myns")
})

local({
  out <- cpp4r:::cpp_decorations(files = test_path("single.cpp"), is_attribute = TRUE)
  expect_equal(out$line, 1L)
})

local({
  out <- cpp4r:::cpp_decorations(
    files = c(test_path("single.cpp"), test_path("multiple.cpp")),
    is_attribute = TRUE
  )
  expect_equal(length(out$file), 4L)
})

# --- parse_cpp_function ---

local({
  out <- cpp4r:::parse_cpp_function(character())
  expect_equal(length(out$name), 0L)
  expect_equal(names(out), c("name", "return_type", "args"))
})

local({
  out <- cpp4r:::parse_cpp_function("int foo() { return 1; }", is_attribute = FALSE)
  expect_equal(out$name, "foo")
  expect_equal(out$return_type, "int")
  expect_equal(length(out$args$name), 0L)
})

local({
  out <- cpp4r:::parse_cpp_function(
    "double bar(int x, bool y) { return 1.0; }",
    is_attribute = FALSE
  )
  expect_equal(out$name, "bar")
  expect_equal(out$return_type, "double")
  args <- out$args
  expect_equal(args$type, c("int", "bool"))
  expect_equal(args$name, c("x", "y"))
})
