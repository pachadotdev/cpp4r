# cpp4r 1.1.0

* Slight changes to internal constructors ('SEXP' and function) to follow the same style as
  'cpp11'.

# cpp4r 1.0.0

* Full detailed documentation.
* Added over 400 units tests to cover different usual cases and corner cases.
* Tested via Docker on all CRAN supported Linux platforms with additional Mac/Windows testing
  via GitHub Actions.

# cpp4r 0.7.0

* I re-added some conditionals to use newer C++ features when the compiler supports them. These
  can add a small speed gain in some cases.
* The codebase default is C++11, with compile time newer standards detection to use newer features
  and optimizations.
* Adds a check for all supported C++ standards in R >= 4.0.0 (>= CXX17).
* Replaces 'testthat' with 'tinytest' after I detected an incompatibility with C++23.

# cpp4r 0.6.0

* `register()` now captures the C++ namespace of decorated functions and emits fully-qualified declarations and calls, so functions defined inside a `namespace { ... }` block (or written as `ns::fun`) can be registered without further setup.
* Removed the run-time dependency on the `decor`, `tibble`, and `vctrs` packages; the small subset
  of helpers that `register()` relies on is now bundled internally and uses base `data.frame`.
* Adds fast paths that provide slight speed improvements (e.g., avoiding branching)
* Follows, to the best possible extent, the R's C API description from https://github.com/hadley/r-internals.
* Slightly expanded `cpp4r::function`.
* Fixed two empty tests.

# cpp4r 0.5.1

* Luke Tierney pointed out a similar change as in v0.5.0 to add to function.hpp.

# cpp4r 0.5.0

* Avoids non-API methods when handling data frames (thanks to Luke Tierney for reporting this)

# cpp4r 0.4.0

* Clearer documentation about the C++ workflow (i.e., how to use anticonf to specify a C++ standard)
* Allows for default values like `my_fun(int x = 100)` to call `my_fun()` with the same result as
  `my_fun(100L)` from R

# cpp4r 0.3.1

* Added support for implicit conversions for R lists

# cpp4r 0.3.0

* This is the first release on CRAN
* Added `as_logicals()` and `as_strings()` in the same style of `as_doubles()` and `as_integers()`
* Improved memory management for `r_vector` iterators
* Slightly faster than `cpp11`
* `vendor()` and `unvendor()` use `path = NULL` as default to adhere to CRAN policies.

# cpp4r 0.2.0

* Reduced dependencies on R side
* The vignettes are rendered only for the package site to keep the CRAN build minimal

# cpp4r 0.1.0

* Initial release
