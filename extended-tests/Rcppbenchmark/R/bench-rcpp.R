#' @export
add_two_rcpp <- function(A, B) {
  add_two_rcpp_(A, B)
}

#' @export
add_four_rcpp <- function(A, B, C, D) {
  add_four_rcpp_(A, B, C, D)
}

#' @export
multiply_four_rcpp <- function(A, B, C, D) {
  multiply_four_rcpp_(A, B, C, D)
}

#' @export
submatrix_manipulation_rcpp <- function(A, B) {
  submatrix_manipulation_rcpp_(A, B)
}

#' @export
multi_operation_rcpp <- function(A, B, C) {
  multi_operation_rcpp_(A, B, C)
}
