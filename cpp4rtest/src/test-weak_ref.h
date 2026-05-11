#include <testthat.h>

context("weak_ref-C++") {
  test_that("weak_ref wraps key and value") {
    // Key must be a reference object (ENVSXP, EXTPTRSXP, WEAKREFSXP, or
    // R_NilValue). Use a small environment.
    SEXP key = PROTECT(R_NewEnv(R_BaseEnv, 0, 0));
    SEXP val = PROTECT(Rf_ScalarReal(3.14));

    cpp4r::weak_ref wr(key, val);

    expect_true(wr.key() == key);
    expect_true(wr.value() == val);
    expect_true(wr.alive());
    expect_true(TYPEOF(SEXP(wr)) == WEAKREFSXP);

    UNPROTECT(2);
  }

  test_that("weak_ref with R_NilValue key is not alive") {
    // R_NilValue is an allowed key; such a weak ref has no liveness.
    cpp4r::weak_ref wr(R_NilValue, R_NilValue);
    expect_false(wr.alive());
    expect_true(wr.key() == R_NilValue);
    expect_true(wr.value() == R_NilValue);
  }

  test_that("weak_ref rejects non-reference keys") {
    SEXP vec = PROTECT(Rf_allocVector(INTSXP, 1));
    expect_error(cpp4r::weak_ref(vec, R_NilValue));
    UNPROTECT(1);
  }

  test_that("weak_ref::value() returns R_NilValue after key is collected") {
    SEXP val = PROTECT(Rf_ScalarReal(1.0));

    // Unprotected environment: eligible for GC.
    SEXP key = R_NewEnv(R_BaseEnv, 0, 0);
    cpp4r::weak_ref wr(key, val);

    R_gc();

    expect_true(wr.value() == R_NilValue);
    expect_false(wr.alive());

    UNPROTECT(1);
  }

  test_that("weak_ref SEXP conversion round-trips to WEAKREFSXP") {
    SEXP key = PROTECT(R_NewEnv(R_BaseEnv, 0, 0));
    cpp4r::weak_ref wr(key, R_NilValue);

    SEXP raw = SEXP(wr);
    expect_true(TYPEOF(raw) == WEAKREFSXP);
    expect_true(R_WeakRefKey(raw) == key);

    UNPROTECT(1);
  }

  test_that("weak_ref finalizer is called when key is collected") {
    static bool finalizer_called = false;
    finalizer_called = false;

    auto fin = [](SEXP) { finalizer_called = true; };

    // Unprotected environment: will be collected on next GC.
    SEXP key = R_NewEnv(R_BaseEnv, 0, 0);
    cpp4r::weak_ref wr(key, R_NilValue, static_cast<R_CFinalizer_t>(fin), false);

    R_gc();

    expect_true(finalizer_called);
  }
}
