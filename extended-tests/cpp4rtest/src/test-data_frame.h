#include <testthat.h>

context("data_frame-C++") {
  test_that("data_frame works") {
    auto getExportedValue = cpp4r::package("base")["getExportedValue"];
    auto mtcars = getExportedValue("datasets", "mtcars");
    cpp4r::data_frame mtcars_df(mtcars);

    expect_true(mtcars_df.nrow() == 32);
    expect_true(mtcars_df.ncol() == 11);

    cpp4r::strings names(mtcars_df.names());
    expect_true(names[0] == "mpg");
    expect_true(names[7] == "vs");

    auto iris = getExportedValue("datasets", "iris");
    cpp4r::data_frame iris_df(iris);

    expect_true(iris_df.nrow() == 150);
    expect_true(iris_df.ncol() == 5);
  }

  test_that("data_frame::nrow works with 0x0 dfs") {
    SEXP x = PROTECT(Rf_allocVector(VECSXP, 0));

    cpp4r::data_frame df(x);
    expect_true(df.nrow() == 0);

    UNPROTECT(1);
  }

  test_that("data_frame::nrow works with 10x0 dfs") {
    using namespace cpp4r::literals;
    cpp4r::writable::list x(0_xl);
    x.attr(R_RowNamesSymbol) = {NA_INTEGER, -10};

    cpp4r::data_frame df(x);
    expect_true(df.nrow() == 10);
  }

  test_that("writable::data_frame::nrow works with 0x0 dfs") {
    SEXP x = PROTECT(Rf_allocVector(VECSXP, 0));

    cpp4r::writable::data_frame df(x);
    expect_true(df.nrow() == 0);

    UNPROTECT(1);
  }

  test_that("writable::data_frame::nrow works with 10x0 dfs (#272)") {
    SEXP x = PROTECT(Rf_allocVector(VECSXP, 0));

    bool is_altrep = false;
    R_xlen_t nrow = 10;

    // Manually specify `nrow` using special constructor
    cpp4r::writable::data_frame df(x, is_altrep, nrow);
    expect_true(df.nrow() == 10);

    UNPROTECT(1);
  }

  test_that("writable::data_frame works") {
    using namespace cpp4r::literals;
    cpp4r::writable::data_frame df({"x"_nm = {1, 2, 3}, "y"_nm = {"a", "b", "c"}});
    // auto nrows = df.nrow();
    expect_true(df.nrow() == 3);
    expect_true(df.ncol() == 2);

    cpp4r::strings names(df.names());
    expect_true(names[0] == "x");
    expect_true(names[1] == "y");

    cpp4r::integers x(df[0]);
    expect_true(x[0] == 1);
    expect_true(x[1] == 2);
    expect_true(x[2] == 3);

    cpp4r::strings y(df[1]);
    expect_true(y[0] == "a");
    expect_true(y[1] == "b");
    expect_true(y[2] == "c");

    SEXP out = df;

    std::string clazz(
        Rf_translateCharUTF8(STRING_ELT(Rf_getAttrib(out, R_ClassSymbol), 0)));
    expect_true(clazz == "data.frame");

    cpp4r::integers row_names(Rf_getAttrib(out, R_RowNamesSymbol));
    expect_true(row_names[0] == 1);
    expect_true(row_names[1] == 2);
    expect_true(row_names[2] == 3);
  }

  test_that("can set attributes on a data_frame") {
    using namespace cpp4r::literals;

    cpp4r::writable::data_frame df({"x"_nm = {1, 2, 3}, "y"_nm = {"a", "b", "c"}});

    df.attr("foo") = "bar";

    cpp4r::r_string foo = cpp4r::strings(df.attr("foo"))[0];
    expect_true(foo == "bar");

    df.names() = {"a", "b"};

    expect_true(cpp4r::integers(df["a"])[0] == 1);
    expect_true(cpp4r::strings(df["b"])[2] == "c");
  }

  test_that("growing vectors uses proper length") {
    using namespace cpp4r::literals;

    cpp4r::writable::integers x, y;
    for (int i = 0; i < 10; ++i) {
      x.push_back(i);
      y.push_back(i);
    }
    cpp4r::writable::data_frame out({"x"_nm = x, "y"_nm = y});

    expect_true(out.nrow() == 10);
  }
}
