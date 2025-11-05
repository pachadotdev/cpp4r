## usethis namespace: start
#' @useDynLib cpp4rsimplexphase2, .registration = TRUE
## usethis namespace: end
NULL

#' Print the table of the simplex algorithm
#' @param c vector of coefficients of the objective function
#' @param b vector of the right hand side of the constraints
#' @param A matrix of the coefficients of the constraints
#' @export
cpp4r_simplex_phase2 <- function(c, b, A) {
  cpp4r_simplex_phase2_(c, b, A)
}
