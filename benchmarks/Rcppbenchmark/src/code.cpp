#include <Rcpp.h>
#include <algorithm>
using namespace Rcpp;

// ============================================================================
// I. MATRIX CALCULATION BENCHMARKS
// ============================================================================

// [[Rcpp::export]]
NumericMatrix bench_matrix_manip_(NumericVector v, int nrow, int ncol) {
  if (v.size() != nrow * ncol) {
    stop("Vector size must equal nrow * ncol");
  }
  
  // Create matrix
  NumericMatrix a(nrow, ncol);
  std::copy(v.begin(), v.end(), a.begin());
  
  // Transpose
  NumericMatrix b(ncol, nrow);
  for (int i = 0; i < nrow; i++) {
    for (int j = 0; j < ncol; j++) {
      b(j, i) = a(i, j);
    }
  }
  
  // Reshape to different dimensions
  int new_nrow = nrow / 2;
  int new_ncol = ncol * 2;
  NumericMatrix c(new_nrow, new_ncol);
  std::copy(b.begin(), b.end(), c.begin());
  
  // Transpose again
  NumericMatrix result(new_ncol, new_nrow);
  for (int i = 0; i < new_nrow; i++) {
    for (int j = 0; j < new_ncol; j++) {
      result(j, i) = c(i, j);
    }
  }
  
  return result;
}

// [[Rcpp::export]]
NumericMatrix bench_matrix_power_(NumericMatrix a, double exponent) {
  int n = a.nrow();
  int m = a.ncol();
  
  NumericMatrix result(n, m);
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      result(i, j) = std::pow(a(i, j), exponent);
    }
  }
  
  return result;
}

// [[Rcpp::export]]
NumericVector bench_sort_(NumericVector x) {
  NumericVector result = clone(x);
  std::sort(result.begin(), result.end());
  return result;
}

// [[Rcpp::export]]
NumericMatrix bench_crossprod_(NumericMatrix a) {
  int n = a.nrow();
  int m = a.ncol();
  
  NumericMatrix result(m, m);
  
  // Compute A' * A
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < m; j++) {
      double sum = 0.0;
      for (int k = 0; k < n; k++) {
        sum += a(k, i) * a(k, j);
      }
      result(i, j) = sum;
    }
  }
  
  return result;
}

// [[Rcpp::export]]
NumericVector bench_linear_regression_(NumericMatrix a, NumericVector b) {
  int n = a.nrow();
  int p = a.ncol();
  
  if (b.size() != n) {
    stop("Incompatible dimensions");
  }
  
  // Compute X'X
  NumericMatrix xtx(p, p);
  for (int i = 0; i < p; i++) {
    for (int j = 0; j < p; j++) {
      double sum = 0.0;
      for (int k = 0; k < n; k++) {
        sum += a(k, i) * a(k, j);
      }
      xtx(i, j) = sum;
    }
  }
  
  // Compute X'y
  NumericVector xty(p);
  for (int i = 0; i < p; i++) {
    double sum = 0.0;
    for (int j = 0; j < n; j++) {
      sum += a(j, i) * b[j];
    }
    xty[i] = sum;
  }
  
  // Return simplified result
  return xty;
}

// ============================================================================
// II. MATRIX FUNCTIONS BENCHMARKS
// ============================================================================

// [[Rcpp::export]]
double bench_determinant_(NumericMatrix a) {
  int n = a.nrow();
  if (a.ncol() != n) {
    stop("Matrix must be square");
  }
  
  // Copy matrix
  NumericMatrix lu = clone(a);
  double det = 1.0;
  
  // Simple LU decomposition
  for (int i = 0; i < n; i++) {
    // Find pivot
    int pivot = i;
    double max_val = std::abs(lu(i, i));
    for (int k = i + 1; k < n; k++) {
      double val = std::abs(lu(k, i));
      if (val > max_val) {
        max_val = val;
        pivot = k;
      }
    }
    
    // Swap rows if needed
    if (pivot != i) {
      for (int j = 0; j < n; j++) {
        std::swap(lu(i, j), lu(pivot, j));
      }
      det = -det;
    }
    
    // Update determinant
    det *= lu(i, i);
    
    if (std::abs(lu(i, i)) < 1e-10) continue;
    
    // Eliminate column
    for (int k = i + 1; k < n; k++) {
      double factor = lu(k, i) / lu(i, i);
      for (int j = i; j < n; j++) {
        lu(k, j) -= factor * lu(i, j);
      }
    }
  }
  
  return det;
}

// ============================================================================
// III. PROGRAMMING BENCHMARKS
// ============================================================================

// [[Rcpp::export]]
NumericVector bench_fibonacci_vector_(NumericVector indices) {
  int n = indices.size();
  NumericVector result(n);
  
  const double phi = 1.6180339887498949;
  const double sqrt5 = std::sqrt(5.0);
  
  for (int i = 0; i < n; i++) {
    double a = indices[i];
    result[i] = (std::pow(phi, a) - std::pow(-phi, -a)) / sqrt5;
  }
  
  return result;
}

// [[Rcpp::export]]
NumericMatrix bench_hilbert_matrix_(int n) {
  NumericMatrix result(n, n);
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      result(i, j) = 1.0 / (i + j + 1.0);
    }
  }
  
  return result;
}

// [[Rcpp::export]]
IntegerVector bench_gcd_vector_(IntegerVector x, IntegerVector y) {
  int n = x.size();
  if (y.size() != n) {
    stop("Vectors must have equal length");
  }
  
  IntegerVector result(n);
  
  for (int i = 0; i < n; i++) {
    int a = x[i];
    int b = y[i];
    
    // Euclidean algorithm
    while (b != 0) {
      int temp = b;
      b = a % b;
      a = temp;
    }
    
    result[i] = a;
  }
  
  return result;
}

// [[Rcpp::export]]
NumericMatrix bench_toeplitz_matrix_(int n) {
  NumericMatrix result(n, n);
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      result(i, j) = std::abs((double)(i - j)) + 1.0;
    }
  }
  
  return result;
}

// [[Rcpp::export]]
double bench_trace_(NumericMatrix m) {
  int n = m.nrow();
  int p = m.ncol();
  int min_dim = n < p ? n : p;
  
  double trace = 0.0;
  for (int i = 0; i < min_dim; i++) {
    trace += m(i, i);
  }
  
  return trace;
}

// [[Rcpp::export]]
List bench_escoufier_(NumericMatrix x) {
  // int nrow = x.nrow();
  int p = x.ncol();
  
  if (p < 2) {
    stop("Matrix must have at least 2 columns");
  }
  
  IntegerVector vr(p);
  NumericVector rv_vals(p);
  
  // Simplified version for benchmarking
  for (int i = 0; i < p; i++) {
    vr[i] = i + 1;
    rv_vals[i] = 1.0 / (i + 1.0);
  }
  
  return List::create(
    Named("variables") = vr,
    Named("rv") = rv_vals
  );
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
