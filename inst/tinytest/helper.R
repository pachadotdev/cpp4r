local_package <- function() {
  dir <- tempfile()
  dir.create(dir)
  do.call(
    "on.exit",
    list(bquote(unlink(.(dir), recursive = TRUE)), TRUE),
    envir = parent.frame()
  )
  writeLines("Package: testPkg", file.path(dir, "DESCRIPTION"))
  writeLines("useDynLib(testPkg, .registration = TRUE)", file.path(dir, "NAMESPACE"))
  desc::desc(dir)
}

pkg_path <- function(pkg) {
  dirname(pkg$.__enclos_env__$private$path)
}

get_funs <- function(path) {
  all_decorations <- cpp4r:::cpp_decorations(path, is_attribute = TRUE)
  cpp4r:::get_registered_functions(all_decorations, "cpp4r::register", quiet = TRUE)
}

get_package_name <- function(path) {
  desc::desc_get("Package", file = file.path(path, "DESCRIPTION"))
}

read_file <- function(x) {
  readChar(x, file.size(x))
}

test_path <- function(name) {
  system.file("tinytest", name, package = "cpp4r")
}
