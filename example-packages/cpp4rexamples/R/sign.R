#' @title Sign of a number
#' @description Returns 1 if the number is positive, -1 if negative, and 0 if zero.
#' @param x A numeric value.
#' @return An integer indicating the sign of the number.
#' @examples
#' sign_r(10)
#' sign_r(-5)
#' sign_r(0)
#' @export
sign_r <- function(x) {
  if (x > 0) {
    1
  } else if (x == 0) {
    0
  } else {
    -1
  }
}
