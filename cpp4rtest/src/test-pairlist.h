#include <testthat.h>

context("pairlist-C++") {
  test_that("pairlist(R_NilValue) is empty") {
    cpp4r::pairlist pl(R_NilValue);

    expect_true(pl.empty());
    expect_true(pl.size() == 0);
    expect_true(pl.begin() == pl.end());
  }

  test_that("pairlist(R_MissingArg) is treated as empty") {
    cpp4r::pairlist pl(R_MissingArg);

    expect_true(pl.empty());
    expect_true(pl.size() == 0);
  }

  test_that("pairlist iterates over a single unnamed element") {
    SEXP val = PROTECT(Rf_ScalarInteger(42));
    SEXP cons = PROTECT(Rf_cons(val, R_NilValue));

    cpp4r::pairlist pl(cons);

    expect_true(pl.size() == 1);
    expect_false(pl.empty());

    auto it = pl.begin();
    expect_true((*it).value == val);
    expect_true((*it).tag == R_NilValue);
    expect_true((*it).tag_name() == nullptr);

    ++it;
    expect_true(it == pl.end());

    UNPROTECT(2);
  }

  test_that("pairlist iterates over named elements") {
    SEXP v1 = PROTECT(Rf_ScalarInteger(1));
    SEXP v2 = PROTECT(Rf_ScalarReal(2.5));

    SEXP node2 = PROTECT(Rf_cons(v2, R_NilValue));
    SET_TAG(node2, Rf_install("b"));
    SEXP node1 = PROTECT(Rf_cons(v1, node2));
    SET_TAG(node1, Rf_install("a"));

    cpp4r::pairlist pl(node1);

    expect_true(pl.size() == 2);

    auto it = pl.begin();
    expect_true((*it).value == v1);
    expect_true(std::string((*it).tag_name()) == "a");

    ++it;
    expect_true((*it).value == v2);
    expect_true(std::string((*it).tag_name()) == "b");

    ++it;
    expect_true(it == pl.end());

    UNPROTECT(4);
  }

  test_that("pairlist range-for loop works") {
    SEXP v1 = PROTECT(Rf_ScalarInteger(1));
    SEXP v2 = PROTECT(Rf_ScalarInteger(2));
    SEXP v3 = PROTECT(Rf_ScalarInteger(3));

    SEXP node3 = PROTECT(Rf_cons(v3, R_NilValue));
    SEXP node2 = PROTECT(Rf_cons(v2, node3));
    SEXP node1 = PROTECT(Rf_cons(v1, node2));

    cpp4r::pairlist pl(node1);

    int count = 0;
    for (auto node : pl) {
      (void)node;
      ++count;
    }
    expect_true(count == 3);

    UNPROTECT(6);
  }

  test_that("pairlist::to_list() returns named VECSXP") {
    SEXP v1 = PROTECT(Rf_ScalarInteger(10));
    SEXP v2 = PROTECT(Rf_ScalarReal(3.14));

    SEXP node2 = PROTECT(Rf_cons(v2, R_NilValue));
    SET_TAG(node2, Rf_install("y"));
    SEXP node1 = PROTECT(Rf_cons(v1, node2));
    SET_TAG(node1, Rf_install("x"));

    cpp4r::pairlist pl(node1);
    cpp4r::list lst = pl.to_list();

    expect_true(lst.size() == 2);
    expect_true(lst["x"] == v1);
    expect_true(lst["y"] == v2);

    UNPROTECT(4);
  }

  test_that("pairlist::to_list() on empty pairlist returns empty list") {
    cpp4r::pairlist pl(R_NilValue);
    cpp4r::list lst = pl.to_list();

    expect_true(lst.size() == 0);
    expect_true(TYPEOF(SEXP(lst)) == VECSXP);
  }

  test_that("pairlist rejects non-pairlist SEXPs") {
    SEXP vec = PROTECT(Rf_allocVector(INTSXP, 3));
    expect_error(cpp4r::pairlist(vec));
    UNPROTECT(1);
  }

  test_that("pairlist SEXP conversion round-trips") {
    SEXP v1 = PROTECT(Rf_ScalarInteger(7));
    SEXP cons = PROTECT(Rf_cons(v1, R_NilValue));

    cpp4r::pairlist pl(cons);
    SEXP back = SEXP(pl);

    expect_true(back == cons);

    UNPROTECT(2);
  }
}
