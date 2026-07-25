#' @title Obtain the OLS Estimator With a Naive Computation
#' @description How to use 'cpp4r' and obtain the OLS estimator in the most naive way. There is a
#'  good explanation about why R's lm() function uses a QR decomposition instead of directly
#'  computing (X'X)-1(X'Y) as I did here. See
#'  \url{http://madrury.github.io/jekyll/update/statistics/2016/07/20/lm-in-R.html}.
#' @useDynLib cpp4rols, .registration = TRUE
"_PACKAGE"

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
