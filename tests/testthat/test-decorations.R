describe("cpp_files", {
  it("returns empty character for empty pkg argument", {
    expect_equal(cpp_files(""), character())
  })

  it("returns empty character when src/ does not exist", {
    tmp <- tempfile()
    dir.create(tmp)
    on.exit(unlink(tmp, recursive = TRUE))
    expect_equal(cpp_files(tmp), character())
  })

  it("lists only C++ files from src/", {
    tmp <- tempfile()
    dir.create(file.path(tmp, "src"), recursive = TRUE)
    on.exit(unlink(tmp, recursive = TRUE))
    file.create(file.path(tmp, "src", "foo.cpp"))
    file.create(file.path(tmp, "src", "bar.cc"))
    file.create(file.path(tmp, "src", "baz.R"))
    files <- cpp_files(tmp)
    expect_true(all(grepl("[.](cpp|cc|h|hpp)$", files)))
    expect_false(any(grepl("[.]R$", files)))
    expect_equal(length(files), 2L)
  })

  it("returns files in stable order", {
    tmp <- tempfile()
    dir.create(file.path(tmp, "src"), recursive = TRUE)
    on.exit(unlink(tmp, recursive = TRUE))
    file.create(file.path(tmp, "src", "z.cpp"))
    file.create(file.path(tmp, "src", "a.cpp"))
    files <- cpp_files(tmp)
    expect_equal(basename(files), c("a.cpp", "z.cpp"))
  })
})

describe("blank_comments", {
  it("leaves plain code unchanged", {
    expect_equal(blank_comments("int x = 1;"), "int x = 1;")
  })

  it("blanks single-line // comments", {
    out <- blank_comments("int x = 1; // comment\nint y = 2;")
    expect_false(grepl("comment", out))
    expect_true(grepl("int y = 2;", out))
  })

  it("blanks multi-line /* */ comments", {
    out <- blank_comments("/* secret\nstuff */ int x;")
    expect_false(grepl("secret", out))
    expect_true(grepl("int x;", out))
  })

  it("preserves newlines inside multi-line comments", {
    out <- blank_comments("/* line1\nline2 */")
    expect_equal(lengths(regmatches(out, gregexpr("\n", out))), 1L)
  })

  it("does not blank content inside string literals", {
    code <- 'const char* s = "// not a comment";'
    out <- blank_comments(code)
    expect_true(grepl("not a comment", out))
  })
})

describe("namespace_per_line", {
  it("returns empty strings for code with no namespaces", {
    lines <- c("int foo() {", "  return 1;", "}")
    expect_equal(namespace_per_line(lines), c("", "", ""))
  })

  it("detects a single namespace", {
    lines <- c(
      "namespace myns {",
      "int foo() { return 1; }",
      "}"
    )
    result <- namespace_per_line(lines)
    expect_equal(result[[1L]], "")
    expect_equal(result[[2L]], "myns")
  })

  it("detects nested namespaces", {
    lines <- c(
      "namespace outer {",
      "namespace inner {",
      "int foo();",
      "}",
      "}"
    )
    result <- namespace_per_line(lines)
    expect_equal(result[[3L]], "outer::inner")
    expect_equal(result[[5L]], "")
  })

  it("returns empty strings for empty input", {
    expect_equal(namespace_per_line(character()), character())
  })
})

describe("cpp_attribute_pattern", {
  it("matches an attribute on its own line", {
    pat <- cpp_attribute_pattern(is_attribute = TRUE)
    expect_true(grepl(pat, "[[cpp4r::register]]"))
  })

  it("matches a comment decoration", {
    pat <- cpp_attribute_pattern(is_attribute = FALSE)
    expect_true(grepl(pat, "// [[cpp4r::register]]"))
  })

  it("does not match plain code", {
    pat <- cpp_attribute_pattern(is_attribute = TRUE)
    expect_false(grepl(pat, "int foo() { return 1; }"))
  })
})

describe("cpp_decorations", {
  it("returns empty list for no files", {
    out <- cpp_decorations(files = character())
    expect_equal(length(out$file), 0L)
    expect_equal(names(out), c("file", "line", "decoration", "namespace", "params", "context"))
  })

  it("returns empty list for non-existent file", {
    out <- cpp_decorations(files = tempfile())
    expect_equal(length(out$file), 0L)
  })

  it("returns empty list for empty file", {
    f <- tempfile(fileext = ".cpp")
    file.create(f)
    on.exit(unlink(f))
    out <- cpp_decorations(files = f, is_attribute = TRUE)
    expect_equal(length(out$file), 0L)
  })

  it("finds a single attribute decoration", {
    f <- tempfile(fileext = ".cpp")
    on.exit(unlink(f))
    writeLines("[[cpp4r::register]] int foo() { return 1; }", f)
    out <- cpp_decorations(files = f, is_attribute = TRUE)
    expect_equal(length(out$file), 1L)
    expect_equal(out$decoration, "cpp4r::register")
    expect_equal(out$namespace, "")
  })

  it("finds multiple attribute decorations", {
    out <- cpp_decorations(files = test_path("multiple.cpp"), is_attribute = TRUE)
    expect_equal(length(out$file), 3L)
    expect_equal(out$decoration, rep("cpp4r::register", 3L))
  })

  it("captures namespace for decorated functions inside a namespace block", {
    f <- tempfile(fileext = ".cpp")
    on.exit(unlink(f))
    writeLines(c(
      "namespace myns {",
      "[[cpp4r::register]]",
      "int fun(int x) { return x; }",
      "}"
    ), f)
    out <- cpp_decorations(files = f, is_attribute = TRUE)
    expect_equal(length(out$file), 1L)
    expect_equal(out$namespace, "myns")
  })

  it("returns correct line numbers", {
    out <- cpp_decorations(files = test_path("single.cpp"), is_attribute = TRUE)
    expect_equal(out$line, 1L)
  })

  it("stacks results from multiple files", {
    out <- cpp_decorations(files = c(test_path("single.cpp"), test_path("multiple.cpp")), is_attribute = TRUE)
    expect_equal(length(out$file), 4L)
  })
})

describe("parse_cpp_function", {
  it("returns empty list for empty context", {
    out <- parse_cpp_function(character())
    expect_equal(length(out$name), 0L)
    expect_equal(names(out), c("name", "return_type", "args"))
  })

  it("parses a simple no-arg function", {
    out <- parse_cpp_function("int foo() { return 1; }", is_attribute = FALSE)
    expect_equal(out$name, "foo")
    expect_equal(out$return_type, "int")
    expect_equal(length(out$args$name), 0L)
  })

  it("parses a function with arguments", {
    out <- parse_cpp_function("double bar(int x, bool y) { return 1.0; }", is_attribute = FALSE)
    expect_equal(out$name, "bar")
    expect_equal(out$return_type, "double")
    args <- out$args
    expect_equal(args$type, c("int", "bool"))
    expect_equal(args$name, c("x", "y"))
  })

  it("parses a function with a default argument", {
    out <- parse_cpp_function("int baz(int x = 0) { return x; }", is_attribute = FALSE)
    args <- out$args
    expect_equal(args$default, "0")
  })

  it("parses an attribute-style decoration on the same line", {
    out <- parse_cpp_function("[[cpp4r::register]] int foo() { return 1; }", is_attribute = TRUE)
    expect_equal(out$name, "foo")
    expect_equal(out$return_type, "int")
  })

  it("handles a void argument list", {
    out <- parse_cpp_function("int foo(void) { return 1; }", is_attribute = FALSE)
    expect_equal(length(out$args$name), 0L)
  })
})

describe("parse_cpp_arguments", {
  it("returns empty data.frame for empty args", {
    out <- parse_cpp_arguments("")
    expect_equal(length(out$name), 0L)
  })

  it("returns empty data.frame for void", {
    out <- parse_cpp_arguments("void")
    expect_equal(length(out$name), 0L)
  })

  it("parses a single argument", {
    out <- parse_cpp_arguments("int x")
    expect_equal(out$type, "int")
    expect_equal(out$name, "x")
    expect_true(is.na(out$default))
  })

  it("parses multiple arguments", {
    out <- parse_cpp_arguments("int x, double y")
    expect_equal(out$type, c("int", "double"))
    expect_equal(out$name, c("x", "y"))
  })

  it("parses a default value", {
    out <- parse_cpp_arguments("int x = 0")
    expect_equal(out$default, "0")
  })

  it("handles pointer and reference types", {
    out <- parse_cpp_arguments("int* p, double& r")
    expect_equal(out$name, c("p", "r"))
    expect_true(grepl("int", out$type[[1L]]))
    expect_true(grepl("double", out$type[[2L]]))
  })

  it("handles template types with commas", {
    out <- parse_cpp_arguments("std::vector<int> v")
    expect_equal(out$name, "v")
    expect_equal(trimws(out$type), "std::vector<int>")
  })

  it("errors when an argument has no type", {
    expect_error(parse_cpp_arguments("x"), "has no type")
  })
})

describe("namespace support in get_registered_functions", {
  it("sets cpp_name to ns::fun for a function inside a namespace block", {
    f <- tempfile(fileext = ".cpp")
    on.exit(unlink(f))
    writeLines(c(
      "namespace myns {",
      "[[cpp4r::register]]",
      "int fun(int x) { return x; }",
      "}"
    ), f)
    decs <- cpp_decorations(files = f, is_attribute = TRUE)
    res <- get_registered_functions(decs, "cpp4r::register")
    expect_equal(res$name, "fun")
    expect_equal(res$cpp_name, "myns::fun")
    expect_equal(res$namespace, "myns")
  })

  it("uses plain name for global-scope function", {
    decs <- cpp_decorations(files = test_path("single.cpp"), is_attribute = TRUE)
    res <- get_registered_functions(decs, "cpp4r::register")
    expect_equal(res$name, "foo")
    expect_equal(res$cpp_name, "foo")
  })
})
