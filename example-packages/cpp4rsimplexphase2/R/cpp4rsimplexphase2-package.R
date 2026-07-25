#' @title A Naive Implementation of the Simplex Phase 2 Algorithm
#' @description Just to demonstrate how to print outputs and do some simple linear
#'  algebra. This is an extremely naive implementation.
#' @useDynLib cpp4rsimplexphase2, .registration = TRUE
"_PACKAGE"

#' Print the table of the simplex algorithm
#' @param c vector of coefficients of the objective function
#' @param b vector of the right hand side of the constraints
#' @param A matrix of the coefficients of the constraints
#' @export
cpp4r_simplex_phase2 <- function(c, b, A) {
  cpp4r_simplex_phase2_(c, b, A)
}
