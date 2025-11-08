#' Unvendor the cpp4r headers
#'
#' This function removes the vendored cpp4r headers from your package by
#' automatically finding the vendored headers.
#'
#' @param path The directory with the vendored headers. It is recommended to use `"./src/vendor"`.
#' The default is `NULL`.
#' @return The path to the unvendored code (invisibly).
#' @export
#' @examples
#' # create a new directory
#' dir <- paste0(tempdir(), "/", gsub("\\s+|[[:punct:]]", "", Sys.time()))
#' dir.create(dir, recursive = TRUE)
#'
#' # vendor the cpp4r headers into the directory
#' vendor(dir)
#'
#' # unvendor the cpp4r headers from the directory
#' unvendor(dir)
#'
#' # cleanup
#' unlink(dir, recursive = TRUE)
unvendor <- function(path = NULL) {
  stopifnot(!is.null(path), dir.exists(path))

  path <- normalizePath(path)

  # Check if the cpp4r directory exists
  cpp4r_dir <- file.path(path, "cpp4r")
  cpp4r_hpp_path <- file.path(path, "cpp4r.hpp")
  info_file_path <- file.path(path, "00-cpp4r-vendoring-info.txt")

  # Check if vendored files exist
  has_cpp4r_dir <- dir.exists(cpp4r_dir)
  has_cpp4r_hpp <- file.exists(cpp4r_hpp_path)
  has_info_file <- file.exists(info_file_path)

  if (!has_cpp4r_dir && !has_cpp4r_hpp && !has_info_file) {
    if (is_interactive()) {
      message("Could not find vendored headers")
    }
    return(invisible(NULL))
  }

  # Remove the cpp4r directory if it exists
  if (has_cpp4r_dir) {
    unlink(cpp4r_dir, recursive = TRUE)
  }

  # Remove cpp4r.hpp if it exists
  if (has_cpp4r_hpp) {
    unlink(cpp4r_hpp_path)
  }

  # Remove the info file if it exists
  unlink(info_file_path)
  
  # If path does not contain any other files, remove the directory
  remaining_files <- list.files(path, all.files = TRUE, no.. = TRUE)
  if (length(remaining_files) == 0) {
    unlink(path, recursive = TRUE)
  }

  if (is_interactive()) {
    message("Unvendored cpp4r from '", path, "'")
    message("DESCRIPTION should link to cpp4r (e.g., 'LinkingTo: cpp4r')")
  }

  invisible(path)
}
