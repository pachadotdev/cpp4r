// Helper functions for R test coverage
// These functions are exposed to R and exercise various cpp4r features

#pragma once

#include <algorithm>
#include <cpp4r.hpp>
#include <numeric>

using namespace cpp4r;

// Integer operations
[[cpp4r::register]]
writable::integers cpp4r_add_int_vec_(integers x, int value) {
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

[[cpp4r::register]]
integers cpp4r_as_integers_(doubles x) {
  return as_integers(x);
}

// Logical operations
[[cpp4r::register]]
writable::logicals cpp4r_negate_logical_(logicals x) {
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

[[cpp4r::register]]
writable::integers cpp4r_logical_to_int_(logicals x) {
  return as_integers(x);
}

[[cpp4r::register]]
writable::doubles cpp4r_logical_to_dbl_(logicals x) {
  return as_doubles(x);
}

// Iterator operations
[[cpp4r::register]]
double cpp4r_iterator_sum_(doubles x) {
  return std::accumulate(x.begin(), x.end(), 0.0);
}

[[cpp4r::register]]
int cpp4r_iterator_sum_int_(integers x) {
  return std::accumulate(x.begin(), x.end(), 0);
}

[[cpp4r::register]]
int cpp4r_iterator_find_(doubles x, double value) {
  auto it = std::find(x.begin(), x.end(), value);
  if (it == x.end()) {
    return -1;
  }
  return static_cast<int>(std::distance(x.begin(), it)) + 1;  // 1-indexed
}

[[cpp4r::register]]
int cpp4r_iterator_count_(doubles x, double value) {
  return static_cast<int>(std::count(x.begin(), x.end(), value));
}

[[cpp4r::register]]
double cpp4r_iterator_min_(doubles x) {
  if (x.size() == 0) return NA_REAL;
  return *std::min_element(x.begin(), x.end());
}

[[cpp4r::register]]
double cpp4r_iterator_max_(doubles x) {
  if (x.size() == 0) return NA_REAL;
  return *std::max_element(x.begin(), x.end());
}

[[cpp4r::register]]
writable::doubles cpp4r_iterator_double_values_(doubles x) {
  writable::doubles result(x.size());
  std::transform(x.begin(), x.end(), result.begin(), [](double v) { return v * 2; });
  return result;
}

[[cpp4r::register]]
writable::doubles cpp4r_reverse_vector_(doubles x) {
  writable::doubles result(x.size());
  auto it = x.end();
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    --it;
    result[i] = *it;
  }
  return result;
}

[[cpp4r::register]]
double cpp4r_iterator_at_(doubles x, int index) {
  auto it = x.begin() + index;
  return *it;
}

[[cpp4r::register]]
int cpp4r_iterator_distance_(doubles x) {
  return static_cast<int>(std::distance(x.begin(), x.end()));
}

// Raw operations
[[cpp4r::register]]
writable::raws cpp4r_raw_copy_(raws x) {
  writable::raws result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    result[i] = x[i];
  }
  return result;
}

[[cpp4r::register]]
writable::raws cpp4r_raw_xor_(raws x, raws mask) {
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
