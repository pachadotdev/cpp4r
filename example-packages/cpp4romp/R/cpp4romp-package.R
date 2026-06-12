## usethis namespace: start
#' @useDynLib cpp4romp, .registration = TRUE
## usethis namespace: end
NULL

#' Unnamed list with squared numbers and the threads used
#' @param x A vector of doubles
#' @export
squared_unnamed <- function(x) {
  squared_unnamed_(as.double(x))
}

#' Named list with squared numbers and the threads used
#' @param x A vector of doubles
#' @export
squared_named <- function(x) {
  squared_named_(as.double(x))
}
