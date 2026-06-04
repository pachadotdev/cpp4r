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

// Helper headers for R-side tinytest coverage
#include "sexp_helpers.h"
#include "weak_ref_helpers.h"
#include "pairlist_helpers.h"
