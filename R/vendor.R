#' @title Vendor the cpp4r headers
#'
#' @description Vendoring is the act of making your own copy of the 3rd party packages your
#' project is using. It is often used in the go language community.
#'
#' This function vendors cpp4r into your package by copying the cpp4r
#' headers into the `inst/include` folder of your package and adding
#' 'cpp4r version: XYZ' to the top of the files, where XYZ is the version of
#' cpp4r currently installed on your machine.
#'
#' **Note**: vendoring places the responsibility of updating the code on
#' **you**. Bugfixes and new features in cpp4r will not be available for your
#' code until you run `cpp_vendor()` again.
#'
#' @param path The directory with the vendored headers. It is recommended to use `"./src/vendor"`.
#' The default is `NULL`.
#' @return The path to the vendored code (invisibly).
#' @export
#' @examples
#' # create a new directory
#' dir <- paste0(tempdir(), "/", gsub("\\s+|[[:punct:]]", "", Sys.time()))
#' dir.create(dir, recursive = TRUE, showWarnings = FALSE)
#'
#' # vendor the cpp4r headers into the directory
#' vendor(dir)
#'
#' list.files(dir, recursive = TRUE)
#'
#' # cleanup
#' unlink(dir, recursive = TRUE)
vendor <- function(path = NULL) {
  if (is.null(path)) {
    stop("You must provide a path to vendor the code into", call. = FALSE)
  }

  path2 <- file.path(path, "cpp4r")
  if (dir.exists(path2)) {
    stop("'", path2, "' already exists\n * run unlink('", path2, "', recursive = TRUE)", call. = FALSE)
  }

  # Vendor cpp4r ----

  dir.create(
    path2,
    recursive = TRUE,
    showWarnings = FALSE
  )

  current_cpp4r <- system.file(
    "include",
    "cpp4r",
    package = "cpp4r"
  )

  if (!nzchar(current_cpp4r)) {
    stop("cpp4r is not installed", call. = FALSE)
  }

  cpp4r_version <- utils::packageVersion("cpp4r")

  writeLines(
    sprintf(
      "cpp4r version: %s\nvendored on: %s",
      cpp4r_version,
      Sys.Date()
    ),
    file.path(path, "00-cpp4r-vendoring-info.txt")
  )

  file.copy(
    system.file("include", "cpp4r.hpp", package = "cpp4r"),
    file.path(path, "cpp4r.hpp"),
    overwrite = TRUE
  )

  cpp4r_files <- list.files(current_cpp4r, full.names = TRUE)

  file.copy(
    cpp4r_files,
    path2,
    overwrite = TRUE
  )

  if (is_interactive()) {
    message("Vendored cpp4r to '", path, "'")

    message(paste(
      "Makevars.in and/or Makevars.win should have a line such as",
      "'PKG_CPPFLAGS = -I vendor/' (default) or 'PKG_CPPFLAGS = -I ../inst/include/'"
    ))

    message("DESCRIPTION should not link to cpp4r (e.g., 'LinkingTo: cpp4r') or the build will ignore the vendored code")
  }

  invisible(path)
}
