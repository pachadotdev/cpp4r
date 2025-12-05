# The function itself just echos its inputs and outputs to a file called INDEX,
# which is then opened by RStudio when the new project is opened.

#' @title Start a new project with the cpp4r package template
#'
#' @description This function copies a package template into a new directory.
#' The template includes a DESCRIPTION file, a minimal R/ directory and placeholders
#' with instructions. You can then edit these files to customize your new package.
#'
#' @param path Path to the new project
#' @param pkgname Name of the new package
#' @return The file path to the copied template (invisibly).
#' @examples
#' # create a new directory
#' dir <- tempdir()
#' dir.create(dir)
#'
#' # copy the package template into the directory
#' pkg_template(dir, "mynewpkg")
#' @export
pkg_template <- function(path = NULL, pkgname = NULL) {
  if (is.null(path)) {
    stop("You must provide a path to the new project", call. = FALSE)
  }
  if (is.null(pkgname)) {
    pkgname <- basename(path)
  }

  # ensure path exists
  dir.create(path, recursive = TRUE, showWarnings = FALSE)

  # copy files
  file.copy(
    list.files(
      system.file("extdata/pkgtemplate", "", package = "cpp4r"),
      full.names = TRUE
    ),
    path,
    recursive = TRUE
  )

  dir.create(paste0(path, "/R"), recursive = TRUE, showWarnings = FALSE)

  lines <- c(
    "^.*\\.Rproj$",
    "^\\.Rproj\\.user$",
    "^\\.Renviron$",
    "^docs$",
    "^vignettes/images$",
    "^\\.github$",
    "^\\.vscode$",
    "^dev$",
    "^LICENSE\\.md$",
    "^README$",
    "^README\\.Rmd$"
  )

  writeLines(lines, con = paste0(path, "/.Rbuildignore"))

  lines <- c(
    paste0("#' @useDynLib ", pkgname, ", .registration = TRUE"),
    "#' @keywords internal",
    "\"_PACKAGE\""
  )

  writeLines(lines, con = paste0(path, "/R/", pkgname, "-package.R"))

  # get roxygen version
  if (!requireNamespace("roxygen2", quietly = TRUE)) {
    stop("You must install the roxygen2 package to use this function", call. = FALSE)
  } else {
    roxyver <- as.character(utils::packageVersion("roxygen2"))
  }

  lines <- c(
    paste("Package:", pkgname),
    "Type: Package",
    "Title: ADD TITLE",
    "Version: 0.1",
    "Authors@R: c(",
    "    person(",
    "        given = \"YOUR\",",
    "        family = \"NAME\",",
    "        role = c(\"aut\", \"cre\"),",
    "        email = \"YOUR@EMAIL.COM\",",
    "        comment = c(ORCID = \"0000-0001-0002-0003\"))",
    "    )",
    "Suggests: ",
    "    knitr,",
    "    rmarkdown",
    "Depends: R(>= 4.0.0)",
    "Description: ADD DESCRIPTION. TWO OR MORE LINES",
    "License: ADD LICENSE",
    "BugReports: https://github.com/USERNAME/PKGNAME/issues",
    "URL: https://WEBSITE.COM",
    # "RoxygenNote: 7.3.0",
    paste0("RoxygenNote: ", roxyver),
    "Encoding: UTF-8",
    "NeedsCompilation: yes",
    "VignetteBuilder: knitr",
    "LinkingTo: cpp4r"
  )

  writeLines(lines, con = paste0(path, "/DESCRIPTION"))

  invisible(path)
}
