#include <testthat.h>

#include <deque>
#include <list>
#include <map>
#include <unordered_map>

context("as_advanced-C++") {
  test_that("as_cpp with int type") {
    SEXP x = PROTECT(Rf_ScalarInteger(42));
    auto val = cpp4r::as_cpp<int>(x);
    expect_true(val == 42);
    UNPROTECT(1);
  }

  test_that("as_sexp with std::map<std::string, SEXP>") {
    std::map<std::string, SEXP> m;
    m["a"] = PROTECT(Rf_ScalarInteger(1));
    m["b"] = PROTECT(Rf_ScalarInteger(2));

    SEXP result = PROTECT(cpp4r::as_sexp(m));
    expect_true(Rf_isVector(result));
    expect_true(Rf_xlength(result) == 2);

    SEXP names = Rf_getAttrib(result, R_NamesSymbol);
    expect_true(names != R_NilValue);
    UNPROTECT(3);
  }

  test_that("as_sexp with std::map<double, int>") {
    std::map<double, int> m;
    m[1.5] = 10;
    m[2.5] = 20;

    SEXP result = PROTECT(cpp4r::as_sexp(m));
    expect_true(Rf_isVector(result));
    UNPROTECT(1);
  }

  test_that("as_sexp with std::unordered_map<std::string, SEXP>") {
    std::unordered_map<std::string, SEXP> m;
    m["x"] = PROTECT(Rf_ScalarReal(1.5));
    m["y"] = PROTECT(Rf_ScalarReal(2.5));

    SEXP result = PROTECT(cpp4r::as_sexp(m));
    expect_true(Rf_isVector(result));
    UNPROTECT(3);
  }

  test_that("as_sexp with std::list<double>") {
    std::list<double> l = {1.0, 2.0, 3.0};
    SEXP result = PROTECT(cpp4r::as_sexp(l));
    expect_true(Rf_isReal(result));
    expect_true(Rf_xlength(result) == 3);
    UNPROTECT(1);
  }

  test_that("as_cpp with SEXP passthrough") {
    SEXP x = PROTECT(Rf_ScalarInteger(42));
    SEXP y = cpp4r::as_cpp<SEXP>(x);
    expect_true(x == y);
    UNPROTECT(1);
  }

  test_that("as_cpp for std::complex<double>") {
    SEXP x = PROTECT(Rf_allocVector(CPLXSXP, 1));
    COMPLEX(x)[0].r = 1.0;
    COMPLEX(x)[0].i = 2.0;

    std::complex<double> c = cpp4r::as_cpp(x);
    expect_true(c.real() == 1.0);
    expect_true(c.imag() == 2.0);
    UNPROTECT(1);
  }

  test_that("as_sexp for std::complex<double>") {
    std::complex<double> c(3.0, 4.0);
    SEXP result = PROTECT(cpp4r::as_sexp(c));
    expect_true(Rf_isComplex(result));
    expect_true(COMPLEX(result)[0].r == 3.0);
    expect_true(COMPLEX(result)[0].i == 4.0);
    UNPROTECT(1);
  }
}
