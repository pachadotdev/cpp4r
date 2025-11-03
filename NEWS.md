# cpp4r 0.4.0

* Clearer documentation about the C++ workflow (i.e., how to use anticonf to specify a C++ standard)
* Allows for default values like `my_fun(int x = 100)` to call `my_fun()` with the same result as `my_fun(100L)` from R

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
