#include <cpp4r.hpp>

#include <numeric>  // for std::accumulate
#include <deque>    // for std::deque
#include <Rmath.h>  // for Rf_rgamma, Rf_rnorm

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

// Test headers (these contain testthat tests, not exported functions)
#include "test-as.h"
#include "test-complex.h"
#include "test-data_frame.h"
#include "test-doubles.h"
#include "test-environment.h"
#include "test-external_pointer.h"
#include "test-function.h"
#include "test-integers.h"
#include "test-list.h"
#include "test-list_of.h"
#include "test-logicals.h"
#include "test-matrix.h"
#include "test-nas.h"
#include "test-protect-nested.h"
#include "test-protect.h"
#include "test-r_complex.h"
#include "test-r_vector.h"
#include "test-raws.h"
#include "test-sexp.h"
#include "test-string.h"
#include "test-strings.h"
