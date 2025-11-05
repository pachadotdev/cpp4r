# R-Benchmark-25 style wrappers for Rcpp

#' @export
bench_matrix_manip <- function(v, nrow, ncol) {
  bench_matrix_manip_(v, nrow, ncol)
}

#' @export
bench_matrix_power <- function(a, exponent) {
  bench_matrix_power_(a, exponent)
}

#' @export
bench_sort <- function(x) {
  bench_sort_(x)
}

#' @export
bench_crossprod <- function(a) {
  bench_crossprod_(a)
}

#' @export
bench_linear_regression <- function(a, b) {
  bench_linear_regression_(a, b)
}

#' @export
bench_determinant <- function(a) {
  bench_determinant_(a)
}

#' @export
bench_fibonacci_vector <- function(indices) {
  bench_fibonacci_vector_(indices)
}

#' @export
bench_hilbert_matrix <- function(n) {
  bench_hilbert_matrix_(n)
}

#' @export
bench_gcd_vector <- function(x, y) {
  bench_gcd_vector_(x, y)
}

#' @export
bench_toeplitz_matrix <- function(n) {
  bench_toeplitz_matrix_(n)
}

#' @export
bench_trace <- function(m) {
  bench_trace_(m)
}

#' @export
bench_escoufier <- function(x) {
  bench_escoufier_(x)
}
