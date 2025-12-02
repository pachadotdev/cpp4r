#include <cpp4r.hpp>

#include <R.h>      // RNG state functions
#include <Rmath.h>  // for Rf_rgamma, Rf_rnorm
#include <deque>    // for std::deque
#include <numeric>  // for std::accumulate

using namespace cpp4r;

// Include all test function headers
#include "add.h"
#include "data_frame.h"
#include "errors.h"
#include "errors_fmt.h"
#include "find-intervals.h"
#include "grow.h"
#include "insert.h"
#include "lists.h"
#include "map.h"
#include "matrix.h"
#include "protect.h"
#include "release.h"
#include "roxygen1.h"
#include "roxygen2.h"
#include "roxygen3.h"
#include "safe.h"
#include "strings.h"
#include "sum.h"
#include "sum_int.h"
#include "truncate.h"

// Test helper functions for R tests
#include "env-helpers.h"
#include "list-complex-helpers.h"
#include "test-helpers.h"

// Test headers (these contain testthat tests, not exported functions)
#include "test-as.h"
#include "test-as_advanced.h"
#include "test-as_complexes.h"
#include "test-attribute_proxy.h"
#include "test-complex.h"
#include "test-data_frame.h"
#include "test-doubles.h"
#include "test-environment.h"
#include "test-environment_advanced.h"
#include "test-external_pointer.h"
#include "test-function.h"
#include "test-function_advanced.h"
#include "test-integers.h"
#include "test-list.h"
#include "test-list_of.h"
#include "test-logicals.h"
#include "test-matrix.h"
#include "test-matrix_advanced.h"
#include "test-named_arg.h"
#include "test-nas.h"
#include "test-protect-nested.h"
#include "test-protect.h"
#include "test-protect_advanced.h"
#include "test-r_bool_advanced.h"
#include "test-r_complex.h"
#include "test-r_string_advanced.h"
#include "test-r_vector.h"
#include "test-r_vector_advanced.h"
#include "test-raws.h"
#include "test-sexp.h"
#include "test-sexp_advanced.h"
#include "test-string.h"
#include "test-strings.h"
#include "test-translate_names.h"

// C++ standard specific tests
#include "test-cxx17.h"
#include "test-cxx20.h"
#include "test-cxx23.h"

// Comprehensive iterator tests
#include "test-iterators-comprehensive.h"

// Extended coverage tests
#include "test-logicals-extended.h"
#include "test-r_vector_fwd-extended.h"
#include "test-raws-extended.h"
