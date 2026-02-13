#include <testthat.h>

context("matrix-advanced-C++") {
  test_that("integers_matrix works") {
    cpp4r::writable::integers_matrix<> m(3, 2);
    m(0, 0) = 1;
    m(1, 0) = 2;
    m(2, 0) = 3;
    m(0, 1) = 4;
    m(1, 1) = 5;
    m(2, 1) = 6;

    expect_true(m(0, 0) == 1);
    expect_true(m(2, 1) == 6);
    expect_true(m.nrow() == 3);
    expect_true(m.ncol() == 2);
  }

  test_that("logicals_matrix works") {
    cpp4r::writable::logicals_matrix<> m(2, 2);
    m(0, 0) = TRUE;
    m(1, 0) = FALSE;
    m(0, 1) = TRUE;
    m(1, 1) = FALSE;

    expect_true(m(0, 0) == TRUE);
    expect_true(m(1, 0) == FALSE);
  }

  test_that("strings_matrix works") {
    SEXP mat = PROTECT(Rf_allocMatrix(STRSXP, 2, 2));
    SET_STRING_ELT(mat, 0, Rf_mkChar("a"));
    SET_STRING_ELT(mat, 1, Rf_mkChar("b"));
    SET_STRING_ELT(mat, 2, Rf_mkChar("c"));
    SET_STRING_ELT(mat, 3, Rf_mkChar("d"));

    cpp4r::strings_matrix<> m(mat);
    expect_true(m(0, 0) == "a");
    expect_true(m(1, 1) == "d");
    UNPROTECT(1);
  }

  test_that("complexes_matrix works") {
    cpp4r::writable::complexes_matrix<> m(2, 2);
    m(0, 0) = cpp4r::r_complex(1, 1);
    m(1, 1) = cpp4r::r_complex(2, 2);

    expect_true(m(0, 0) == cpp4r::r_complex(1, 1));
    expect_true(m(1, 1) == cpp4r::r_complex(2, 2));
  }

  test_that("matrix slice iteration") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];
    cpp4r::doubles_matrix<cpp4r::by_column> m(getExportedValue("datasets", "volcano"));

    int count = 0;
    for (auto slice : m) {
      count++;
      expect_true(slice.size() == 87);
    }
    expect_true(count == 61);
  }

  test_that("matrix slice element iteration") {
    // Use read-only matrix for slice iteration to avoid pointer-to-reference issue
    SEXP mat = PROTECT(Rf_allocMatrix(REALSXP, 3, 2));
    REAL(mat)[0] = 1;
    REAL(mat)[1] = 2;
    REAL(mat)[2] = 3;
    REAL(mat)[3] = 4;
    REAL(mat)[4] = 5;
    REAL(mat)[5] = 6;

    cpp4r::doubles_matrix<cpp4r::by_column> m(mat);
    auto col0 = m[0];
    double sum = 0;
    for (auto elem : col0) {
      sum += static_cast<double>(elem);
    }
    expect_true(sum == 6.0);
    UNPROTECT(1);
  }

  test_that("matrix attr operations") {
    cpp4r::writable::doubles_matrix<> m(2, 2);
    m.attr("custom") = "value";

    SEXP attr_val = Rf_getAttrib(m.data(), Rf_install("custom"));
    expect_true(strcmp(CHAR(STRING_ELT(attr_val, 0)), "value") == 0);
  }

  test_that("matrix attr with initializer_list") {
    cpp4r::writable::doubles_matrix<> m(2, 2);
    SEXP a = PROTECT(Rf_ScalarInteger(1));
    SEXP b = PROTECT(Rf_ScalarInteger(2));
    m.attr("test", {a, b});

    SEXP attr_val = Rf_getAttrib(m.data(), Rf_install("test"));
    expect_true(Rf_isVector(attr_val));
    expect_true(Rf_xlength(attr_val) == 2);
    UNPROTECT(2);
  }
}
