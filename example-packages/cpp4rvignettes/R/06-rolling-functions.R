#' @title Cumulative Sum
#' @description Returns a vector of doubles containing the cumulative sum of a vector of doubles.
#' @param x doubles (numeric) vector.
#' @export
cumsum_r <- function(x) {
  n <- length(x)
  out <- numeric(n)
  out[1] <- x[1]
  for (i in 2:n) {
    out[i] <- out[i - 1] + x[i]
  }
  out
}

#' @title Cumulative Product
#' @description Returns a vector of doubles containing the cumulative product of a vector of doubles.
#' @param x doubles (numeric) vector.
#' @export
cumprod_r <- function(x) {
  n <- length(x)
  out <- numeric(n)
  out[1] <- x[1]
  for (i in 2:n) {
    out[i] <- out[i - 1] * x[i]
  }
  out
}
