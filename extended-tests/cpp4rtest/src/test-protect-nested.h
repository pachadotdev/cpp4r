#include "testthat.h"

/*
 * See https://github.com/r-lib/cpp4r/pull/327 for full details.
 *
 * - `cpp4r::package("cpp4rtest")["test_destruction_outer"]` uses
 *   `unwind_protect()` to call R level `test_destruction_outer()` but no entry
 *   macros are set up. Instead we are going to catch exceptions that get here
 *   with `expect_error_as()`.
 *
 *   - Call R level `test_destruction_outer()` to set up `BEGIN_CPP4R` /
 *     `END_CPP4R` entry macros.
 *
 *   - C++ `test_destruction_outer()` goes through `unwind_protect()` to call
 *     the R level `test_destruction_inner()`.
 *
 *     - R level `test_destruction_inner()` sets up its own `BEGIN_CPP4R` /
 *       `END_CPP4R` entry macros.
 *
 *     - C++ `test_destruction_inner()` goes through `unwind_protect()` to call
 *       `Rf_error()` (i.e., we are nested within `unwind_protect()`s!).
 *
 *     - `longjmp()` is caught from inner `unwind_protect()`, and an exception
 *       is thrown which is caught by the inner entry macros, allowing us to run
 *       the destructor of `x`, then we let R continue the unwind process.
 *
 *   - This `longjmp()`s again and is caught by the outer `unwind_protect()`, an
 *     exception is thrown which is caught by the outer entry macros, and we let
 *     R continue the unwind process one more time.
 *
 * - This `longjmp()` is caught by `cpp4r::package()`'s `unwind_protect()`,
 *   an exception is thrown, and that is caught by `expect_error_as()`.
 */

// Global variable to detect if the destructor has been run or not
static bool destructed = false;

class HasDestructor {
 public:
  ~HasDestructor();
};

HasDestructor::~HasDestructor() {
  // Destructor has run!
  destructed = true;
}

[[cpp4r::register]] void test_destruction_inner() {
  // Expect that `x`'s destructor gets to run on the way out
  HasDestructor x{};
  cpp4r::stop("oh no!");
}

[[cpp4r::register]] void test_destruction_outer() {
  const auto test_destruction_inner =
      cpp4r::package("cpp4rtest")["test_destruction_inner"];
  test_destruction_inner();
}

context("unwind_protect-nested-C++") {
  test_that(
      "nested `unwind_protect()` (with entry macros set up) will run destructors"
      "(#327)") {
    const auto fn = [&] {
      const auto test_destruction_outer =
          cpp4r::package("cpp4rtest")["test_destruction_outer"];
      test_destruction_outer();
    };

    expect_error_as(fn(), cpp4r::unwind_exception);
    expect_true(destructed);

    destructed = false;
  }
}
