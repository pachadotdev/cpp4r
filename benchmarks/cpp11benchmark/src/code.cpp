#include <R_ext/Random.h>
#include "Rmath.h"
#include <cmath>
#include <vector>
#include <cpp11.hpp>

using namespace cpp11;

[[cpp11::register]] double bench_prealloc_weighted_sum_(doubles x, doubles y, doubles w) {
  int n = x.size();
  if (y.size() != n || w.size() != n) {
    stop("Vectors must have equal length");
  }
  
  // Direct pointer access for performance
  const double* px = REAL_OR_NULL(x.data());
  const double* py = REAL_OR_NULL(y.data());
  const double* pw = REAL_OR_NULL(w.data());
  
  double result = 0.0;
  for (int i = 0; i < n; i++) {
    result += pw[i] * (px[i] + py[i]);
  }
  
  return result;
}

[[cpp11::register]] doubles_matrix<> bench_matrix_multiply_(doubles_matrix<> A, doubles_matrix<> B) {
  int n = A.nrow();
  int m = A.ncol();
  int p = B.ncol();
  
  if (B.nrow() != m) {
    stop("Incompatible dimensions for matrix multiplication");
  }
  
  // Pre-allocate result matrix
  writable::doubles_matrix<> C(n, p);
  
  // Direct pointer access
  const double* pA = REAL_OR_NULL(A.data());
  const double* pB = REAL_OR_NULL(B.data());
  double* pC = REAL(C.data());
  
  // Initialize result to zero
  for (int i = 0; i < n * p; i++) {
    pC[i] = 0.0;
  }
  
  // Matrix multiplication (column-major order, cache-friendly)
  for (int j = 0; j < p; j++) {
    for (int k = 0; k < m; k++) {
      for (int i = 0; i < n; i++) {
        pC[i + j * n] += pA[i + k * n] * pB[k + j * m];
      }
    }
  }
  
  return C;
}

[[cpp11::register]] doubles bench_rolling_mean_(doubles x, int w) {
  int n = x.size();

  if (w <= 0 || w > n) {
    stop("Invalid w size");
  }
  
  // Pre-allocate result (same size as input, with NAs at start)
  writable::doubles result(n);
  double* pr = REAL(result.data());
  const double* px = REAL_OR_NULL(x.data());
  
  // First (w-1) elements are NA
  for (int i = 0; i < w - 1; i++) {
    pr[i] = NA_REAL;
  }
  
  // Compute rolling mean efficiently using running sum
  double sum = 0.0;
  for (int i = 0; i < w; i++) {
    sum += px[i];
  }
  pr[w - 1] = sum / w;
  
  for (int i = w; i < n; i++) {
    sum = sum - px[i - w] + px[i];
    pr[i] = sum / w;
  }
  
  return result;
}

[[cpp11::register]] data_frame bench_dataframe_summary_(data_frame tbl) {
  int ncols = tbl.size();
  
  // Count numeric columns
  int n_numeric = 0;
  for (int j = 0; j < ncols; j++) {
    if (TYPEOF(tbl[j]) == REALSXP) {
      n_numeric++;
    }
  }
  
  if (n_numeric == 0) {
    stop("No numeric columns in data frame");
  }
  
  // Pre-allocate result vectors
  writable::strings col_names(n_numeric);
  writable::doubles means(n_numeric);
  writable::doubles sds(n_numeric);
  writable::doubles mins(n_numeric);
  writable::doubles maxs(n_numeric);
  
  int idx = 0;
  for (int j = 0; j < ncols; j++) {
    if (TYPEOF(tbl[j]) == REALSXP) {
      doubles col(tbl[j]);
      const double* p = REAL_OR_NULL(col.data());
      int n = col.size();
      
      // Compute statistics
      double sum = 0.0, sum_sq = 0.0;
      double min_val = R_PosInf, max_val = R_NegInf;
      
      for (int i = 0; i < n; i++) {
        double val = p[i];
        sum += val;
        sum_sq += val * val;
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
      }
      
      double mean = sum / n;
      double variance = (sum_sq - sum * sum / n) / (n - 1);
      double sd = std::sqrt(variance);
      
      col_names[idx] = static_cast<const char*>(CHAR(STRING_ELT(tbl.names().data(), j)));
      means[idx] = mean;
      sds[idx] = sd;
      mins[idx] = min_val;
      maxs[idx] = max_val;
      idx++;
    }
  }
  
  // Create result data frame
  writable::list result_list(5);
  result_list[0] = col_names;
  result_list[1] = means;
  result_list[2] = sds;
  result_list[3] = mins;
  result_list[4] = maxs;
  
  writable::strings names(5);
  names[0] = "column";
  names[1] = "mean";
  names[2] = "sd";
  names[3] = "min";
  names[4] = "max";
  result_list.names() = names;
  
  writable::data_frame result_tbl(result_list);
  return result_tbl;
}

[[cpp11::register]] int bench_string_pattern_count_(strings s, strings p) {
  int n = s.size();
  int total_count = 0;

  // pattern passed as length-1 character vector; convert its first element safely
  std::string p_str;
  if (p.size() > 0) {
    r_string rp(p[0]);
    p_str = std::string(rp);
  }

  for (int i = 0; i < n; i++) {
    r_string rs(s[i]);
    std::string str(rs);

    // Count occurrences of pattern in string
    size_t pos = 0;
    while ((pos = str.find(p_str, pos)) != std::string::npos) {
      total_count++;
      pos += p_str.length();
    }
  }

  return total_count;
}

[[cpp11::register]] doubles bench_grouped_mean_(doubles x, integers groups) {
  int n = x.size();
  if (groups.size() != n) {
    stop("values and groups must have same length");
  }
  
  const double* pv = REAL_OR_NULL(x.data());
  const int* pg = INTEGER_OR_NULL(groups.data());
  
  // Find number of unique groups
  int max_group = 0;
  for (int i = 0; i < n; i++) {
    if (pg[i] > max_group) max_group = pg[i];
  }
  
  if (max_group <= 0) {
    stop("Groups must be positive integers");
  }
  
  // Pre-allocate accumulators
  std::vector<double> sums(max_group, 0.0);
  std::vector<int> counts(max_group, 0);
  
  // Accumulate sums and counts
  for (int i = 0; i < n; i++) {
    int g = pg[i] - 1;  // R uses 1-based indexing
    if (g >= 0 && g < max_group) {
      sums[g] += pv[i];
      counts[g]++;
    }
  }
  
  // Compute means
  writable::doubles result(max_group);
  double* pr = REAL(result.data());
  for (int g = 0; g < max_group; g++) {
    pr[g] = counts[g] > 0 ? sums[g] / counts[g] : NA_REAL;
  }
  
  return result;
}

[[cpp11::register]] doubles bench_fibonacci_(int n) {
  if (n <= 0) {
    stop("n must be positive");
  }
  
  writable::doubles result(n);
  double* p = REAL(result.data());
  
  if (n >= 1) p[0] = 1.0;
  if (n >= 2) p[1] = 1.0;
  
  for (int i = 2; i < n; i++) {
    p[i] = p[i-1] + p[i-2];
  }
  
  return result;
}

[[cpp11::register]] double bench_mean_na_rm_(doubles x) {
  int n = x.size();
  const double* p = REAL_OR_NULL(x.data());
  
  double sum = 0.0;
  int count = 0;
  
  for (int i = 0; i < n; i++) {
    if (!ISNA(p[i]) && !std::isnan(p[i])) {
      sum += p[i];
      count++;
    }
  }
  
  return count > 0 ? sum / count : NA_REAL;
}

[[cpp11::register]] doubles_matrix<> bench_outer_product_(doubles x, doubles y) {
  int n = x.size();
  int m = y.size();
  
  // Pre-allocate result matrix
  writable::doubles_matrix<> res(n, m);
  
  const double* px = REAL_OR_NULL(x.data());
  const double* py = REAL_OR_NULL(y.data());
  double* pr = REAL(res.data());
  
  // Fill matrix (column-major order)
  for (int j = 0; j < m; j++) {
    for (int i = 0; i < n; i++) {
      pr[i + j * n] = px[i] * py[j];
    }
  }
  
  return res;
}

[[cpp11::register]] doubles_matrix<> bench_pairwise_distances_(doubles_matrix<> x) {
  int n = x.nrow();
  int k = x.ncol();
  
  // Pre-allocate result matrix
  writable::doubles_matrix<> dist(n, n);
  
  const double* pp = REAL_OR_NULL(x.data());
  double* pd = REAL(dist.data());
  
  // Compute pairwise distances
  for (int i = 0; i < n; i++) {
    pd[i + i * n] = 0.0;  // Diagonal is zero
    
    for (int j = i + 1; j < n; j++) {
      double sum_sq = 0.0;
      
      for (int d = 0; d < k; d++) {
        double diff = pp[i + d * n] - pp[j + d * n];
        sum_sq += diff * diff;
      }
      
      double distance = std::sqrt(sum_sq);
      pd[i + j * n] = distance;
      pd[j + i * n] = distance;  // Symmetric
    }
  }
  
  return dist;
}

[[cpp11::register]] list bench_cumulative_ops_(doubles x) {
  int n = x.size();
  const double* px = REAL_OR_NULL(x.data());
  
  // Pre-allocate all result vectors
  writable::doubles cumsum(n);
  writable::doubles cumprod(n);
  writable::doubles cummin(n);
  writable::doubles cummax(n);
  
  double* ps = REAL(cumsum.data());
  double* pp = REAL(cumprod.data());
  double* pmin = REAL(cummin.data());
  double* pmax = REAL(cummax.data());
  
  if (n > 0) {
    ps[0] = px[0];
    pp[0] = px[0];
    pmin[0] = px[0];
    pmax[0] = px[0];
    
    for (int i = 1; i < n; i++) {
      ps[i] = ps[i-1] + px[i];
      pp[i] = pp[i-1] * px[i];
      pmin[i] = px[i] < pmin[i-1] ? px[i] : pmin[i-1];
      pmax[i] = px[i] > pmax[i-1] ? px[i] : pmax[i-1];
    }
  }
  
  // Return as named list
  writable::list result(4);
  result[0] = cumsum;
  result[1] = cumprod;
  result[2] = cummin;
  result[3] = cummax;
  
  writable::strings names(4);
  names[0] = "cumsum";
  names[1] = "cumprod";
  names[2] = "cummin";
  names[3] = "cummax";
  result.names() = names;
  
  return result;
}

[[cpp11::register]] doubles bench_bootstrap_mean_(doubles x, int n_boot) {
  int n = x.size();
  const double* px = REAL_OR_NULL(x.data());
  
  // Pre-allocate result
  writable::doubles boot_means(n_boot);
  double* pm = REAL(boot_means.data());
  
  // Get R's RNG state
  GetRNGstate();
  
  for (int b = 0; b < n_boot; b++) {
    double sum = 0.0;
    
    // Resample with replacement
    for (int i = 0; i < n; i++) {
      int idx = static_cast<int>(unif_rand() * n);
      if (idx >= n) idx = n - 1;  // Handle edge case
      sum += px[idx];
    }
    
    pm[b] = sum / n;
  }
  
  PutRNGstate();
  
  return boot_means;
}
