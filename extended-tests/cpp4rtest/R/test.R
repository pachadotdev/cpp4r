#' Run the cpp4r tests
#' @inheritParams testthat::test_check
#' @export
run_tests <- function(reporter = testthat::default_reporter()) {
  if (interactive()) {
    if (pkgload::is_dev_package("cpp4r")) {
      # load cpp4r
      pkgload::load_all(system.file(".", package = "cpp4r"))

      # load cpp4rtest
      pkgload::load_all(system.file("cpp4rtest", package = "cpp4r"))
    } else {
      # load cpp4r
      pkgload::load_all("..")

      # load cpp4rtest
      pkgload::load_all()
    }
  }

  old <- getwd()
  on.exit(setwd(old))
  setwd(system.file("tests", package = "cpp4rtest"))

  testthat::test_check("cpp4rtest", reporter = reporter)
}
