#' @title Is Any Element in a Logical Vector "TRUE"
#' @description Loop over a vector and returns "true" when a "true" element is found in the sequence.
#' @param x logical vector.
#' @export
any_r <- function(x) {
  n <- length(x)
  
  for (i in 1:n) {
    if (x[i]) {
      return(TRUE)
    }
  }
  FALSE
}

#' @title Which Elements in a Logical Vector are "TRUE"
#' @description Loop over a vector and returns the positions that are "true". Returns a vector of
#'  integers.
#' @param x logical vector.
#' @export
which_r <- function(x) {
  n <- length(x)
  res <- c()
  j <- 0

  for (i in 1:n) {
    if (x[i]) {
      res <- c(res, i)
      j <- j + 1
    }
  }

  if (j == 0) {
    return(0)
  } else {
    return(res)
  }
}
