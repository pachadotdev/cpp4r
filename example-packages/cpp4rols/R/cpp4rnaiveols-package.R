#' @useDynLib cpp4rols, .registration = TRUE
NULL

#' Naive OLS estimator
#' @export
#' @param X numeric matrix
#' @param Y numeric matrix
#' @return numeric matrix
#' @examples
#' X <- matrix(1, nrow = nrow(mtcars), ncol = 2)
#' X[, 2] <- mtcars$wt
#' Y <- matrix(mtcars$mpg, nrow = nrow(mtcars), ncol = 1)
#' naive_ols(X, Y)
naive_ols <- function(X, Y) {
  naive_ols_(X, Y)
}
