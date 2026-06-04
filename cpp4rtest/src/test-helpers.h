// Helper functions for R test coverage
// These functions are exposed to R and exercise various cpp4r features

#pragma once

#include <algorithm>
#include <cpp4r.hpp>
#include <numeric>

using namespace cpp4r;

/* roxygen
@title Add Integer Value to Integer Vector
@description Test suite
@param x vector of integer numbers (R)
@param value integer value to add
@export
*/
[[cpp4r::register]] writable::integers add_int_vec_(integers x, int value) {
  writable::integers result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    if (is_na(x[i])) {
      result[i] = NA_INTEGER;
    } else {
      result[i] = x[i] + value;
    }
  }
  return result;
}

/* roxygen
@title Convert Double Vector to Integer Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] integers as_integers_(doubles x) { return as_integers(x); }

/* roxygen
@title Convert Logical Vector to Integer Vector
@description Test suite
@param x vector of logical values (R)
@export
*/
[[cpp4r::register]] writable::logicals negate_logical_(logicals x) {
  writable::logicals result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    if (is_na(x[i])) {
      result[i] = NA_LOGICAL;
    } else if (x[i] == r_bool(true)) {
      result[i] = r_bool(false);
    } else {
      result[i] = r_bool(true);
    }
  }
  return result;
}

/* roxygen
@title Convert Logical Vector to Integer Vector
@description Test suite
@param x vector of logical values (R)
@export
*/
[[cpp4r::register]] writable::integers logical_to_int_(logicals x) {
  return as_integers(x);
}

/* roxygen
@title Convert Logical Vector to Double Vector
@description Test suite
@param x vector of logical values (R)
@export
*/
[[cpp4r::register]] writable::doubles logical_to_dbl_(logicals x) {
  return as_doubles(x);
}

/* roxygen
@title Sum Double Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] double iterator_sum_(doubles x) {
  return std::accumulate(x.begin(), x.end(), 0.0);
}

/* roxygen
@title Sum Integer Vector
@description Test suite
@param x vector of integer numbers (R)
@export
*/
[[cpp4r::register]] int iterator_sum_int_(integers x) {
  return std::accumulate(x.begin(), x.end(), 0);
}

/* roxygen
@title Find Value in Double Vector
@description Test suite
@param x vector of double numbers (R)
@param value double value to find
@export
*/
[[cpp4r::register]] int iterator_find_(doubles x, double value) {
  auto it = std::find(x.begin(), x.end(), value);
  if (it == x.end()) {
    return -1;
  }
  return static_cast<int>(std::distance(x.begin(), it)) + 1;  // 1-indexed
}

/* roxygen
@title Count Value in Double Vector
@description Test suite
@param x vector of double numbers (R)
@param value double value to count
@export
*/
[[cpp4r::register]]
int iterator_count_(doubles x, double value) {
  return static_cast<int>(std::count(x.begin(), x.end(), value));
}

/* roxygen
@title Find Minimum Value in Double Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] double iterator_min_(doubles x) {
  if (x.size() == 0) return NA_REAL;
  return *std::min_element(x.begin(), x.end());
}

/* roxygen
@title Find Maximum Value in Double Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] double iterator_max_(doubles x) {
  if (x.size() == 0) return NA_REAL;
  return *std::max_element(x.begin(), x.end());
}

/* roxygen
@title Double Values in Double Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] writable::doubles iterator_double_values_(doubles x) {
  writable::doubles result(x.size());
  std::transform(x.begin(), x.end(), result.begin(), [](double v) { return v * 2; });
  return result;
}

/* roxygen
@title Reverse Double Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] writable::doubles reverse_vector_(doubles x) {
  writable::doubles result(x.size());
  auto it = x.end();
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    --it;
    result[i] = *it;
  }
  return result;
}

/* roxygen
@title Access Element in Double Vector
@description Test suite
@param x vector of double numbers (R)
@param index index of the element to access (1-based)
@export
*/
[[cpp4r::register]] double iterator_at_(doubles x, int index) {
  auto it = x.begin() + index;
  return *it;
}

/* roxygen
@title Get Distance of Double Vector
@description Test suite
@param x vector of double numbers (R)
@export
*/
[[cpp4r::register]] int iterator_distance_(doubles x) {
  return static_cast<int>(std::distance(x.begin(), x.end()));
}

/* roxygen
@title Copy Raw Vector
@description Test suite
@param x vector of raw bytes (R)
@export
*/
[[cpp4r::register]] writable::raws raw_copy_(raws x) {
  writable::raws result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    result[i] = x[i];
  }
  return result;
}

/* roxygen
@title XOR Raw Vector with Mask
@description Test suite
@param x vector of raw bytes (R)
@param mask single byte mask (R)
@export
*/
[[cpp4r::register]] writable::raws raw_xor_(raws x, raws mask) {
  if (mask.size() != 1) {
    stop("mask must be a single byte");
  }
  writable::raws result(x.size());
  Rbyte m = mask[0];
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    result[i] = x[i] ^ m;
  }
  return result;
}
