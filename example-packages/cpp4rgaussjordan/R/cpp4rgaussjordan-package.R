#' @useDynLib cpp4rgaussjordan, .registration = TRUE
NULL

#' Invert (some) square matrices
#' @export
#' @param A numeric matrix
#' @return numeric matrix
#' @examples
#' A <- matrix(c(2,1,3,-1), nrow = 2, ncol = 2)
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
#' A <- matrix(c(2,1,3,-1), nrow = 2, ncol = 2)
#' b <- matrix(c(7,4), nrow = 2, ncol = 1)
#' solve_system(A, b)
solve_system <- function(A,b) {
  solve_system_(A, b)
}
