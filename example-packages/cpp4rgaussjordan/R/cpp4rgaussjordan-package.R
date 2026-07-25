#' @title A Naive Implementation of the Gauss-Jordan Algorithm
#' @description Just to demonstrate the vendoring capabilities of the 'cpp4r'
#'  package. This is a very simple implementation to show code syntax.
#' @useDynLib cpp4rgaussjordan, .registration = TRUE
"_PACKAGE"

#' Invert (some) square matrices
#' @export
#' @param A numeric matrix
#' @return numeric matrix
#' @examples
#' A <- matrix(c(2, 1, 3, -1), nrow = 2, ncol = 2)
#' invert_matrix(A)
invert_matrix <- function(A) {
  invert_matrix_(A)
}

#' Solve (some) linear systems
#' @export
#' @param A numeric matrix
#' @param b numeric matrix
#' @return numeric matrix
#' @examples
#' A <- matrix(c(2, 1, 3, -1), nrow = 2, ncol = 2)
#' b <- matrix(c(7, 4), nrow = 2, ncol = 1)
#' solve_system(A, b)
solve_system <- function(A, b) {
  solve_system_(A, b)
}
