#include <testthat.h>

context("as_complexes-C++") {
  test_that("as_complexes from integers") {
    cpp4r::writable::integers x({1, 2, 3});
    cpp4r::complexes result = cpp4r::as_complexes(x);

    expect_true(result.size() == 3);
    expect_true(result[0] == cpp4r::r_complex(1, 0));
    expect_true(result[1] == cpp4r::r_complex(2, 0));
    expect_true(result[2] == cpp4r::r_complex(3, 0));
  }

  test_that("as_complexes handles NA_INTEGER") {
    cpp4r::writable::integers x({NA_INTEGER, 42});
    cpp4r::complexes result = cpp4r::as_complexes(x);

    expect_true(cpp4r::is_na(result[0]));
    expect_true(result[1] == cpp4r::r_complex(42, 0));
  }

  test_that("as_complexes from complex passthrough") {
    cpp4r::writable::complexes x({cpp4r::r_complex(1, 2)});
    cpp4r::complexes result = cpp4r::as_complexes(x);
    expect_true(result[0] == cpp4r::r_complex(1, 2));
  }

  test_that("as_complexes throws on invalid type") {
    cpp4r::writable::strings x({"a", "b"});
    expect_error(cpp4r::as_complexes(x));
  }

  test_that("complex_vector wrapper works") {
    SEXP x = PROTECT(Rf_allocVector(CPLXSXP, 2));
    COMPLEX(x)[0].r = 1.0;
    COMPLEX(x)[0].i = 2.0;
    COMPLEX(x)[1].r = 3.0;
    COMPLEX(x)[1].i = 4.0;

    cpp4r::complex_vector cv(x);
    expect_true(cv.size() == 2);
    expect_true(cv[0] == std::complex<double>(1.0, 2.0));
    expect_true(cv[1] == std::complex<double>(3.0, 4.0));
    UNPROTECT(1);
  }
}
