#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
double bench_prealloc_weighted_sum_(NumericVector x, NumericVector y, NumericVector w) {
  int n = x.size();
  if (y.size() != n || w.size() != n) {
    stop("Vectors must have equal length");
  }
  
  double result = 0.0;
  for (int i = 0; i < n; i++) {
    result += w[i] * (x[i] + y[i]);
  }
  
  return result;
}

// [[Rcpp::export]]
NumericMatrix bench_matrix_multiply_(NumericMatrix A, NumericMatrix B) {
  int n = A.nrow();
  int m = A.ncol();
  int p = B.ncol();
  
  if (B.nrow() != m) {
    stop("Incompatible dimensions for matrix multiplication");
  }
  
  // Pre-allocate result matrix
  NumericMatrix C(n, p);
  
  // Matrix multiplication (column-major order, cache-friendly)
  for (int j = 0; j < p; j++) {
    for (int k = 0; k < m; k++) {
      for (int i = 0; i < n; i++) {
        C(i, j) += A(i, k) * B(k, j);
      }
    }
  }
  
  return C;
}

// [[Rcpp::export]]
NumericVector bench_rolling_mean_(NumericVector x, int w) {
  int n = x.size();

  if (w <= 0 || w > n) {
    stop("Invalid w size");
  }
  
  // Pre-allocate result (same size as input, with NAs at start)
  NumericVector result(n);
  
  // First (w-1) elements are NA
  for (int i = 0; i < w - 1; i++) {
    result[i] = NA_REAL;
  }
  
  // Compute rolling mean efficiently using running sum
  double sum = 0.0;
  for (int i = 0; i < w; i++) {
    sum += x[i];
  }
  result[w - 1] = sum / w;
  
  for (int i = w; i < n; i++) {
    sum = sum - x[i - w] + x[i];
    result[i] = sum / w;
  }
  
  return result;
}

// [[Rcpp::export]]
DataFrame bench_dataframe_summary_(DataFrame tbl) {
  int ncols = tbl.size();
  CharacterVector tbl_names = tbl.names();
  
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
  CharacterVector col_names(n_numeric);
  NumericVector means(n_numeric);
  NumericVector sds(n_numeric);
  NumericVector mins(n_numeric);
  NumericVector maxs(n_numeric);
  
  int idx = 0;
  for (int j = 0; j < ncols; j++) {
    if (TYPEOF(tbl[j]) == REALSXP) {
      NumericVector col = tbl[j];
      int n = col.size();
      
      // Compute statistics
      double sum = 0.0, sum_sq = 0.0;
      double min_val = R_PosInf, max_val = R_NegInf;
      
      for (int i = 0; i < n; i++) {
        double val = col[i];
        sum += val;
        sum_sq += val * val;
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
      }
      
      double mean = sum / n;
      double variance = (sum_sq - sum * sum / n) / (n - 1);
      double sd = std::sqrt(variance);
      
      col_names[idx] = tbl_names[j];
      means[idx] = mean;
      sds[idx] = sd;
      mins[idx] = min_val;
      maxs[idx] = max_val;
      idx++;
    }
  }
  
  // Create result data frame
  return DataFrame::create(
    Named("column") = col_names,
    Named("mean") = means,
    Named("sd") = sds,
    Named("min") = mins,
    Named("max") = maxs
  );
}

// [[Rcpp::export]]
int bench_string_pattern_count_(CharacterVector s, std::string p) {
  int n = s.size();
  int total_count = 0;
  
  for (int i = 0; i < n; i++) {
    std::string str = as<std::string>(s[i]);
    
    // Count occurrences of pattern in string
    size_t pos = 0;
    while ((pos = str.find(p, pos)) != std::string::npos) {
      total_count++;
      pos += p.length();
    }
  }
  
  return total_count;
}

// [[Rcpp::export]]
NumericVector bench_grouped_mean_(NumericVector x, IntegerVector groups) {
  int n = x.size();
  if (groups.size() != n) {
    stop("values and groups must have same length");
  }
  
  // Find number of unique groups
  int max_group = 0;
  for (int i = 0; i < n; i++) {
    if (groups[i] > max_group) max_group = groups[i];
  }
  
  if (max_group <= 0) {
    stop("Groups must be positive integers");
  }
  
  // Pre-allocate accumulators
  std::vector<double> sums(max_group, 0.0);
  std::vector<int> counts(max_group, 0);
  
  // Accumulate sums and counts
  for (int i = 0; i < n; i++) {
    int g = groups[i] - 1;  // R uses 1-based indexing
    if (g >= 0 && g < max_group) {
      sums[g] += x[i];
      counts[g]++;
    }
  }
  
  // Compute means
  NumericVector result(max_group);
  for (int g = 0; g < max_group; g++) {
    result[g] = counts[g] > 0 ? sums[g] / counts[g] : NA_REAL;
  }
  
  return result;
}

// [[Rcpp::export]]
NumericVector bench_fibonacci_(int n) {
  if (n <= 0) {
    stop("n must be positive");
  }
  
  NumericVector result(n);
  
  if (n >= 1) result[0] = 1.0;
  if (n >= 2) result[1] = 1.0;
  
  for (int i = 2; i < n; i++) {
    result[i] = result[i-1] + result[i-2];
  }
  
  return result;
}

// [[Rcpp::export]]
double bench_mean_na_rm_(NumericVector x) {
  int n = x.size();
  
  double sum = 0.0;
  int count = 0;
  
  for (int i = 0; i < n; i++) {
    if (!NumericVector::is_na(x[i]) && !std::isnan(x[i])) {
      sum += x[i];
      count++;
    }
  }
  
  return count > 0 ? sum / count : NA_REAL;
}

// [[Rcpp::export]]
NumericMatrix bench_outer_product_(NumericVector x, NumericVector y) {
  int n = x.size();
  int m = y.size();
  
  // Pre-allocate result matrix
  NumericMatrix res(n, m);
  
  // Fill matrix (column-major order)
  for (int j = 0; j < m; j++) {
    for (int i = 0; i < n; i++) {
      res(i, j) = x[i] * y[j];
    }
  }
  
  return res;
}

// [[Rcpp::export]]
NumericMatrix bench_pairwise_distances_(NumericMatrix x) {
  int n = x.nrow();
  int k = x.ncol();
  
  // Pre-allocate result matrix
  NumericMatrix dist(n, n);
  
  // Compute pairwise distances
  for (int i = 0; i < n; i++) {
    dist(i, i) = 0.0;  // Diagonal is zero
    
    for (int j = i + 1; j < n; j++) {
      double sum_sq = 0.0;
      
      for (int d = 0; d < k; d++) {
        double diff = x(i, d) - x(j, d);
        sum_sq += diff * diff;
      }
      
      double distance = std::sqrt(sum_sq);
      dist(i, j) = distance;
      dist(j, i) = distance;  // Symmetric
    }
  }
  
  return dist;
}

// [[Rcpp::export]]
List bench_cumulative_ops_(NumericVector x) {
  int n = x.size();
  
  // Pre-allocate all result vectors
  NumericVector cumsum(n);
  NumericVector cumprod(n);
  NumericVector cummin(n);
  NumericVector cummax(n);
  
  if (n > 0) {
    cumsum[0] = x[0];
    cumprod[0] = x[0];
    cummin[0] = x[0];
    cummax[0] = x[0];
    
    for (int i = 1; i < n; i++) {
      cumsum[i] = cumsum[i-1] + x[i];
      cumprod[i] = cumprod[i-1] * x[i];
      cummin[i] = x[i] < cummin[i-1] ? x[i] : cummin[i-1];
      cummax[i] = x[i] > cummax[i-1] ? x[i] : cummax[i-1];
    }
  }
  
  // Return as named list
  return List::create(
    Named("cumsum") = cumsum,
    Named("cumprod") = cumprod,
    Named("cummin") = cummin,
    Named("cummax") = cummax
  );
}

// [[Rcpp::export]]
NumericVector bench_bootstrap_mean_(NumericVector x, int n_boot) {
  int n = x.size();
  
  // Pre-allocate result
  NumericVector boot_means(n_boot);
  
  // Get R's RNG state
  GetRNGstate();
  
  for (int b = 0; b < n_boot; b++) {
    double sum = 0.0;
    
    // Resample with replacement
    for (int i = 0; i < n; i++) {
      int idx = static_cast<int>(unif_rand() * n);
      if (idx >= n) idx = n - 1;  // Handle edge case
      sum += x[idx];
    }
    
    boot_means[b] = sum / n;
  }
  
  PutRNGstate();
  
  return boot_means;
}
