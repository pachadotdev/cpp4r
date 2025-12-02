# Run C++ unit tests
test_that("C++ unit tests run without unexpected errors", {
    # https://stackoverflow.com/a/79565018/3720258
    expect_warnings <- function(object, warnings, ...) {
        wrns <- testthat::capture_warnings({
            act <- testthat::quasi_label(rlang::enquo(object), arg = "object")
        })
        res <- vapply(warnings, function(wrn) any(grepl(wrn, wrns)), FUN.VALUE = logical(1))
        if (all(res)) {
            testthat::succeed()
            return(invisible(act$val))
        } else {
            msg <- sprintf("%s did not produce one or more warnings: %s", act$lab, toString(names(res)[!res]))
            testthat::fail(msg)
        }
        }

    expect_warnings(run_cpp_tests("cpp4rtest"), rep("test", 3))
})
