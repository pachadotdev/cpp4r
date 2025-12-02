#include <testthat.h>

context("matrix-C++") {
  test_that("matrix dim attributes are correct for writable matrices") {
    cpp4r::writable::doubles_matrix<cpp4r::by_row> x(5, 2);

    cpp4r::integers dim(SEXP(x.attr("dim")));

    expect_true(dim[0] == 5);
    expect_true(dim[1] == 2);

    expect_true(x.nrow() == 5);
    expect_true(x.ncol() == 2);
    expect_true(x.nslices() == 5);
    expect_true(x.slice_size() == 2);
    expect_true(x.slice_stride() == 5);
    expect_true(x.slice_offset(0) == 0);
    expect_true(x.slice_offset(1) == 1);
    expect_true(x[1].size() == 2);
    expect_true(x[1].stride() == 5);
  }
  test_that("matrix dim attributes are correct for read only matrices") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];

    test_that("matrix<by_row> attributes are correct") {
      cpp4r::doubles_matrix<cpp4r::by_row> x(getExportedValue("datasets", "volcano"));

      expect_true(x.size() == 5307);
      expect_true(x.nrow() == 87);
      expect_true(x.ncol() == 61);
      expect_true(x.nslices() == 87);
      expect_true(x.slice_size() == 61);
      expect_true(x.slice_stride() == 87);
      expect_true(x.slice_offset(0) == 0);
      expect_true(x.slice_offset(1) == 1);
      expect_true(x[1].size() == 61);
      expect_true(x[1].stride() == 87);
    }
    test_that("matrix<by_column> attributes are correct") {
      cpp4r::doubles_matrix<cpp4r::by_column> x(getExportedValue("datasets", "volcano"));

      expect_true(x.size() == 5307);
      expect_true(x.nrow() == 87);
      expect_true(x.ncol() == 61);
      expect_true(x.nslices() == 61);
      expect_true(x.slice_size() == 87);
      expect_true(x.slice_stride() == 1);
      expect_true(x.slice_offset(0) == 0);
      expect_true(x.slice_offset(1) == 87);
      expect_true(x[1].size() == 87);
      expect_true(x[1].stride() == 1);
    }
  }

  test_that("row based subsetting works") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];

    cpp4r::doubles_matrix<cpp4r::by_row> x(getExportedValue("datasets", "volcano"));
    expect_true(x.nslices() == 87);
    expect_true(x.slice_size() == 61);

    auto r = x[0];
    expect_true(r[0] == 100);
    expect_true(r[60] == 103);

    auto r2 = x[2];
    expect_true(r2[0] == 102);
    expect_true(r2[60] == 104);
  }

  test_that("column based subsetting works") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];

    cpp4r::doubles_matrix<cpp4r::by_column> x(getExportedValue("datasets", "volcano"));
    expect_true(x.nslices() == 61);
    expect_true(x.slice_size() == 87);

    auto c = x[0];
    expect_true(c[0] == 100);
    expect_true(c[86] == 97);

    auto c2 = x[5];
    expect_true(c2[0] == 101);
    expect_true(c2[86] == 99);
  }

  test_that("index based subsetting works") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];

    cpp4r::doubles_matrix<cpp4r::by_row> xr(getExportedValue("datasets", "volcano"));
    expect_true(xr(0, 0) == 100);
    expect_true(xr(0, 60) == 103);
    expect_true(xr(10, 13) == 121);

    cpp4r::doubles_matrix<cpp4r::by_column> xc(getExportedValue("datasets", "volcano"));
    expect_true(xc(0, 0) == 100);
    expect_true(xc(0, 60) == 103);
    expect_true(xc(10, 13) == 121);
  }

  test_that("copy constructor works for read only matrices") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];
    cpp4r::doubles_matrix<cpp4r::by_row> x(getExportedValue("datasets", "volcano"));

    cpp4r::doubles_matrix<cpp4r::by_row> yr(x);
    expect_true(x.nrow() == yr.nrow());
    expect_true(x.ncol() == yr.ncol());
    expect_true(yr.nslices() == yr.nrow());
    expect_true(SEXP(x) == SEXP(yr));

    cpp4r::doubles_matrix<cpp4r::by_column> yc(x);
    expect_true(x.nrow() == yc.nrow());
    expect_true(x.ncol() == yc.ncol());
    expect_true(yc.nslices() == yc.ncol());
    expect_true(SEXP(x) == SEXP(yc));
  }

  test_that("copy constructor works for writable matrices") {
    cpp4r::writable::doubles_matrix<cpp4r::by_row> x(5, 2);

    auto x_dim = x.attr("dim");
    expect_true(INTEGER_ELT(x_dim, 0) == 5);
    expect_true(INTEGER_ELT(x_dim, 1) == 2);

    cpp4r::writable::doubles_matrix<cpp4r::by_row> yr(x);
    expect_true(x.nrow() == yr.nrow());
    expect_true(x.ncol() == yr.ncol());
    expect_true(yr.nslices() == yr.nrow());
    // Note that a copy should be made when copying writable!
    expect_true(SEXP(x) != SEXP(yr));

    // `dim` attribute is retained on copy
    auto yr_dim = yr.attr("dim");
    expect_true(INTEGER_ELT(yr_dim, 0) == 5);
    expect_true(INTEGER_ELT(yr_dim, 1) == 2);

    cpp4r::writable::doubles_matrix<cpp4r::by_column> yc(x);
    expect_true(x.nrow() == yc.nrow());
    expect_true(x.ncol() == yc.ncol());
    expect_true(yc.nslices() == yc.ncol());
    // Note that a copy should be made when copying writable!
    expect_true(SEXP(x) != SEXP(yc));

    // `dim` attribute is retained on copy
    auto yc_dim = yc.attr("dim");
    expect_true(INTEGER_ELT(yc_dim, 0) == 5);
    expect_true(INTEGER_ELT(yc_dim, 1) == 2);
  }

  test_that("copy constructor is not enabled across vector types") {
    cpp4r::writable::doubles_matrix<cpp4r::by_row> x(5, 2);
    expect_error(cpp4r::writable::integers_matrix<cpp4r::by_column>(x));
  }

  test_that("doubles_matrix implicitly coerces integer matrix to double") {
    // Create an integer matrix
    SEXP int_mat = PROTECT(Rf_allocMatrix(INTSXP, 2, 3));
    INTEGER(int_mat)[0] = 1;
    INTEGER(int_mat)[1] = 2;
    INTEGER(int_mat)[2] = 3;
    INTEGER(int_mat)[3] = 4;
    INTEGER(int_mat)[4] = 5;
    INTEGER(int_mat)[5] = 6;

    // Implicit coercion via constructor
    cpp4r::doubles_matrix<> result(int_mat);

    expect_true(result.nrow() == 2);
    expect_true(result.ncol() == 3);
    expect_true(result(0, 0) == 1.0);
    expect_true(result(1, 0) == 2.0);
    expect_true(result(0, 1) == 3.0);
    expect_true(result(1, 1) == 4.0);
    expect_true(result(0, 2) == 5.0);
    expect_true(result(1, 2) == 6.0);

    // Check it's actually double type
    expect_true(cpp4r::detail::r_typeof(result.data()) == REALSXP);

    UNPROTECT(1);
  }

  test_that("doubles_matrix implicit coercion handles NA values correctly") {
    SEXP int_mat = PROTECT(Rf_allocMatrix(INTSXP, 2, 2));
    INTEGER(int_mat)[0] = 1;
    INTEGER(int_mat)[1] = NA_INTEGER;
    INTEGER(int_mat)[2] = 3;
    INTEGER(int_mat)[3] = 4;

    cpp4r::doubles_matrix<> result(int_mat);

    expect_true(result(0, 0) == 1.0);
    expect_true(cpp4r::is_na(result(1, 0)));
    expect_true(result(0, 1) == 3.0);
    expect_true(result(1, 1) == 4.0);

    UNPROTECT(1);
  }

  test_that("doubles_matrix implicit coercion preserves dimnames") {
    // Create integer matrix with dimnames
    SEXP int_mat = PROTECT(Rf_allocMatrix(INTSXP, 2, 2));
    INTEGER(int_mat)[0] = 1;
    INTEGER(int_mat)[1] = 2;
    INTEGER(int_mat)[2] = 3;
    INTEGER(int_mat)[3] = 4;

    SEXP dimnames = PROTECT(Rf_allocVector(VECSXP, 2));
    SEXP rownames = PROTECT(Rf_allocVector(STRSXP, 2));
    SEXP colnames = PROTECT(Rf_allocVector(STRSXP, 2));

    SET_STRING_ELT(rownames, 0, Rf_mkChar("r1"));
    SET_STRING_ELT(rownames, 1, Rf_mkChar("r2"));
    SET_STRING_ELT(colnames, 0, Rf_mkChar("c1"));
    SET_STRING_ELT(colnames, 1, Rf_mkChar("c2"));

    SET_VECTOR_ELT(dimnames, 0, rownames);
    SET_VECTOR_ELT(dimnames, 1, colnames);
    Rf_setAttrib(int_mat, R_DimNamesSymbol, dimnames);

    cpp4r::doubles_matrix<> result(int_mat);

    // Check dimnames are preserved
    SEXP result_dimnames = Rf_getAttrib(result.data(), R_DimNamesSymbol);
    expect_true(result_dimnames != R_NilValue);

    SEXP result_rownames = VECTOR_ELT(result_dimnames, 0);
    SEXP result_colnames = VECTOR_ELT(result_dimnames, 1);

    expect_true(strcmp(CHAR(STRING_ELT(result_rownames, 0)), "r1") == 0);
    expect_true(strcmp(CHAR(STRING_ELT(result_rownames, 1)), "r2") == 0);
    expect_true(strcmp(CHAR(STRING_ELT(result_colnames, 0)), "c1") == 0);
    expect_true(strcmp(CHAR(STRING_ELT(result_colnames, 1)), "c2") == 0);

    UNPROTECT(4);
  }

  test_that("doubles_matrix implicitly coerces logical matrix") {
    SEXP lgl_mat = PROTECT(Rf_allocMatrix(LGLSXP, 2, 2));
    LOGICAL(lgl_mat)[0] = TRUE;
    LOGICAL(lgl_mat)[1] = FALSE;
    LOGICAL(lgl_mat)[2] = TRUE;
    LOGICAL(lgl_mat)[3] = NA_LOGICAL;

    cpp4r::doubles_matrix<> result(lgl_mat);

    expect_true(result(0, 0) == 1.0);
    expect_true(result(1, 0) == 0.0);
    expect_true(result(0, 1) == 1.0);
    expect_true(cpp4r::is_na(result(1, 1)));

    UNPROTECT(1);
  }

  test_that("doubles_matrix rejects incompatible types") {
    SEXP str_mat = PROTECT(Rf_allocMatrix(STRSXP, 2, 2));
    expect_error(cpp4r::doubles_matrix<>(str_mat));
    UNPROTECT(1);
  }

  test_that("integers_matrix implicitly coerces logical matrix") {
    SEXP lgl_mat = PROTECT(Rf_allocMatrix(LGLSXP, 2, 2));
    LOGICAL(lgl_mat)[0] = TRUE;
    LOGICAL(lgl_mat)[1] = FALSE;
    LOGICAL(lgl_mat)[2] = TRUE;
    LOGICAL(lgl_mat)[3] = NA_LOGICAL;

    cpp4r::integers_matrix<> result(lgl_mat);

    expect_true(result(0, 0) == 1);
    expect_true(result(1, 0) == 0);
    expect_true(result(0, 1) == 1);
    expect_true(result(1, 1) == NA_INTEGER);

    UNPROTECT(1);
  }

  test_that("integers_matrix rejects double matrix (narrowing)") {
    SEXP dbl_mat = PROTECT(Rf_allocMatrix(REALSXP, 2, 2));
    REAL(dbl_mat)[0] = 1.0;
    REAL(dbl_mat)[1] = 2.0;
    REAL(dbl_mat)[2] = 3.0;
    REAL(dbl_mat)[3] = 4.0;

    // integers_matrix should NOT accept doubles (would be narrowing conversion)
    expect_error(cpp4r::integers_matrix<>(dbl_mat));

    UNPROTECT(1);
  }

  test_that("complexes_matrix implicitly coerces double matrix") {
    SEXP dbl_mat = PROTECT(Rf_allocMatrix(REALSXP, 2, 2));
    REAL(dbl_mat)[0] = 1.0;
    REAL(dbl_mat)[1] = 2.0;
    REAL(dbl_mat)[2] = 3.0;
    REAL(dbl_mat)[3] = 4.0;

    cpp4r::complexes_matrix<> result(dbl_mat);

    expect_true(result(0, 0).real() == 1.0);
    expect_true(result(1, 0).real() == 2.0);
    expect_true(result(0, 1).real() == 3.0);
    expect_true(result(1, 1).real() == 4.0);

    // Imaginary parts should be 0
    expect_true(result(0, 0).imag() == 0.0);
    expect_true(result(1, 1).imag() == 0.0);

    expect_true(cpp4r::detail::r_typeof(result.data()) == CPLXSXP);

    UNPROTECT(1);
  }

  test_that("complexes_matrix implicitly coerces integer matrix") {
    SEXP int_mat = PROTECT(Rf_allocMatrix(INTSXP, 2, 2));
    INTEGER(int_mat)[0] = 1;
    INTEGER(int_mat)[1] = 2;
    INTEGER(int_mat)[2] = 3;
    INTEGER(int_mat)[3] = 4;

    cpp4r::complexes_matrix<> result(int_mat);

    expect_true(result(0, 0).real() == 1.0);
    expect_true(result(1, 0).real() == 2.0);
    expect_true(result(0, 1).real() == 3.0);
    expect_true(result(1, 1).real() == 4.0);

    UNPROTECT(1);
  }
}
