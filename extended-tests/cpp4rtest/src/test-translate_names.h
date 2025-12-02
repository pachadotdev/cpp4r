#include <testthat.h>

context("translate_names-C++") {
  test_that("translate_names_to_vector works") {
    using namespace cpp4r::literals;
    cpp4r::writable::doubles x({"a"_nm = 1., "b"_nm = 2., "c"_nm = 3.});

    SEXP names = x.names();
    std::vector<std::string> cache = cpp4r::translate_names_to_vector(names);

    expect_true(cache.size() == 3);
    expect_true(cache[0] == "a");
    expect_true(cache[1] == "b");
    expect_true(cache[2] == "c");
  }

  test_that("translate_names_to_vector with R_NilValue") {
    std::vector<std::string> cache = cpp4r::translate_names_to_vector(R_NilValue);
    expect_true(cache.empty());
  }

  test_that("find_name_pos_cached works") {
    std::vector<std::string> names = {"a", "b", "c"};

    expect_true(cpp4r::find_name_pos_cached(names, cpp4r::r_string("a")) == 0);
    expect_true(cpp4r::find_name_pos_cached(names, cpp4r::r_string("b")) == 1);
    expect_true(cpp4r::find_name_pos_cached(names, cpp4r::r_string("c")) == 2);
    expect_true(cpp4r::find_name_pos_cached(names, cpp4r::r_string("d")) == -1);
  }

  test_that("find_cached with read-only vector") {
    using namespace cpp4r::literals;
    // Create a named SEXP first
    SEXP s = PROTECT(Rf_allocVector(REALSXP, 2));
    REAL(s)[0] = 1.;
    REAL(s)[1] = 2.;
    SEXP names = PROTECT(Rf_allocVector(STRSXP, 2));
    SET_STRING_ELT(names, 0, Rf_mkChar("a"));
    SET_STRING_ELT(names, 1, Rf_mkChar("b"));
    Rf_setAttrib(s, R_NamesSymbol, names);

    // Use read-only vector (find_cached is implemented for read-only)
    cpp4r::doubles x(s);
    std::vector<std::string> cache = cpp4r::translate_names_to_vector(names);

    auto it = cpp4r::find_cached(x, cache, cpp4r::r_string("b"));
    expect_true(it != x.end());
    expect_true(*it == 2.);

    auto it2 = cpp4r::find_cached(x, cache, cpp4r::r_string("z"));
    expect_true(it2 == x.end());

    UNPROTECT(2);
  }
}
