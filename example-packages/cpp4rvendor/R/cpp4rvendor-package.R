#' @title To Test Vendored Builds
#' @description Testing the builds with vendored cpp4r headers. The idea is to
#'  ensure that the functions correctly link to the vendored cpp4r code and not
#'  to any installed version of cpp4r.
#' @useDynLib cpp4rvendor, .registration = TRUE
"_PACKAGE"
