#include <cpp4r.hpp>

#include <R.h>      // RNG state functions
#include <Rmath.h>  // for Rf_rgamma, Rf_rnorm
#include <deque>    // for std::deque
#include <numeric>  // for std::accumulate

using namespace cpp4r;

// Include all test function headers
#include "examples/add.h"
#include "examples/data_frame.h"
#include "examples/errors.h"
#include "examples/external-pointers.h"
#include "examples/find-intervals.h"
#include "examples/grow.h"
#include "examples/insert.h"
#include "examples/lists.h"
#include "examples/map.h"
#include "examples/matrix.h"
#include "examples/protect.h"
#include "examples/release.h"
#include "examples/safe.h"
#include "examples/strings.h"
#include "examples/sum.h"
#include "examples/sum_cplx.h"
#include "examples/sum_int.h"
#include "examples/truncate.h"

// Test helper functions for R tests
#include "examples/env-helpers.h"
#include "examples/list-complex-helpers.h"
#include "examples/test-helpers.h"

// Helper headers for R-side tinytest coverage
#include "examples/pairlist_helpers.h"
#include "examples/sexp_helpers.h"
#include "examples/weak_ref_helpers.h"
