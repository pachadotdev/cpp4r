#' @title Demonstration that 'cpp4r' can Use OpenMP with the Right Compiler
#' @description The goal of this package is to demonstrate some features of the 
#'  cpp4r package, and this also demonstrates how to create named and unnamed lists.
#'  Build this package with gcc or clang on any computer with an Intel 
#'  or AMD processor to compile the contained C++ code with OpenMP support. It also
#'  works if you install libomp via Homebrew on macOS.
#'  The Makevars file is set up to use the OpenMP flag for the compiler, and 
#'  commenting its lines generates a build that works, but the functions will 
#'  use one thread.
#' @useDynLib cpp4romp, .registration = TRUE
"_PACKAGE"

#' Unnamed list with squared numbers and the threads used
#' @param x A vector of doubles
#' @export
squared_unnamed <- function(x) {
  squared_unnamed_(as.double(x))
}

#' Named list with squared numbers and the threads used
#' @param x A vector of doubles
#' @export
squared_named <- function(x) {
  squared_named_(as.double(x))
}
