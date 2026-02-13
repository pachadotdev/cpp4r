#include <testthat.h>

context("list_of-C++") {
  test_that("list_of works") {
    using namespace cpp4r::literals;

    cpp4r::writable::list x({"x"_nm = cpp4r::writable::doubles({1., 2., 3.}),
                             "y"_nm = cpp4r::writable::doubles({4., 5., 6.})});

    cpp4r::list_of<cpp4r::doubles> res(x);

    expect_true(res.size() == 2);
    expect_true(res[0][0] == 1.);
    expect_true(res[1][0] == 4.);
    expect_true(res.names()[0] == "x");
    expect_true(res.names()[1] == "y");
  }

  test_that("writable::list_of works") {
    using namespace cpp4r::literals;

    cpp4r::writable::list x({"x"_nm = cpp4r::writable::doubles({1., 2., 3.}),
                             "y"_nm = cpp4r::writable::doubles({4., 5., 6.})});

    cpp4r::writable::list_of<cpp4r::writable::doubles> res(x);

    res.push_back({"z"_nm = cpp4r::writable::doubles({7., 8., 9.})});

    expect_true(res.size() == 3);
    expect_true(REAL(VECTOR_ELT(res, 2))[0] == 7.);

    res[0][0] = 2.;

    expect_true(REAL(VECTOR_ELT(res, 0))[0] == 2.);

    res[1] = cpp4r::writable::doubles({7., 8., 9.});

    expect_true(REAL(res[1])[0] == 7.);
    expect_true(REAL(res[1])[1] == 8.);
    expect_true(REAL(res[1])[2] == 9.);

    res["x"][0] = 4;
    expect_true(REAL(res[0])[0] == 4.);
  }
}
