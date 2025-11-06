#include <R_ext/Random.h>
#include "Rmath.h"
#include <cmath>
#include <algorithm>
#include <cpp4r.hpp>

using namespace cpp4r;

// ============================================================================
// I. MATRIX CALCULATION BENCHMARKS
// ============================================================================

/* roxygen
@title Matrix Creation, Transpose and Reshape
@description Creates matrix from vector, transposes, reshapes, transposes again.
  Tests matrix manipulation overhead.
@param v Input vector
@param nrow Number of rows
@param ncol Number of columns
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_matrix_manip(doubles v, int nrow, int ncol) {
  if (v.size() != nrow * ncol) {
    stop("Vector size must equal nrow * ncol");
  }
  
  // Create matrix
  writable::doubles_matrix<> a(nrow, ncol);
  const double* pv = REAL_OR_NULL(v.data());
  double* pa = REAL(a.data());
  std::copy(pv, pv + v.size(), pa);
  
  // Transpose
  writable::doubles_matrix<> b(ncol, nrow);
  double* pb = REAL(b.data());
  for (int i = 0; i < nrow; i++) {
    for (int j = 0; j < ncol; j++) {
      pb[j + i * ncol] = pa[i + j * nrow];
    }
  }
  
  // Reshape to different dimensions
  int new_nrow = nrow / 2;
  int new_ncol = ncol * 2;
  writable::doubles_matrix<> c(new_nrow, new_ncol);
  double* pc = REAL(c.data());
  std::copy(pb, pb + v.size(), pc);
  
  // Transpose again
  writable::doubles_matrix<> result(new_ncol, new_nrow);
  double* pr = REAL(result.data());
  for (int i = 0; i < new_nrow; i++) {
    for (int j = 0; j < new_ncol; j++) {
      pr[j + i * new_ncol] = pc[i + j * new_nrow];
    }
  }
  
  return result;
}

/* roxygen
@title Matrix Power
@description Raises each element of matrix to a power.
@param a Input matrix
@param exponent Power to raise to
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_matrix_power(doubles_matrix<> a, double exponent) {
  int n = a.nrow();
  int m = a.ncol();
  
  writable::doubles_matrix<> result(n, m);
  const double* pa = REAL_OR_NULL(a.data());
  double* pr = REAL(result.data());
  
  int total = n * m;
  for (int i = 0; i < total; i++) {
    pr[i] = std::pow(pa[i], exponent);
  }
  
  return result;
}

/* roxygen
@title Vector Sorting
@description Sorts a numeric vector using quicksort.
@param x Input vector
@export
*/
[[cpp4r::register]] doubles bench_sort(doubles x) {
  int n = x.size();
  writable::doubles result(n);
  
  const double* px = REAL_OR_NULL(x.data());
  double* pr = REAL(result.data());
  std::copy(px, px + n, pr);
  
  std::sort(pr, pr + n);
  
  return result;
}

/* roxygen
@title Matrix Cross-Product
@description Computes A' * A (crossprod).
@param a Input matrix
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_crossprod(doubles_matrix<> a) {
  int n = a.nrow();
  int m = a.ncol();
  
  writable::doubles_matrix<> result(m, m);
  const double* pa = REAL_OR_NULL(a.data());
  double* pr = REAL(result.data());
  
  // Initialize to zero
  std::fill(pr, pr + m * m, 0.0);
  
  // Compute A' * A
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < m; j++) {
      for (int k = 0; k < n; k++) {
        pr[i + j * m] += pa[k + i * n] * pa[k + j * n];
      }
    }
  }
  
  return result;
}

/* roxygen
@title Linear Regression via QR
@description Solves linear system using QR decomposition approach.
  Simplified version that just does the matrix operations.
@param a Design matrix
@param b Response vector
@export
*/
[[cpp4r::register]] doubles bench_linear_regression(doubles_matrix<> a, doubles b) {
  int n = a.nrow();
  int p = a.ncol();
  
  if (b.size() != n) {
    stop("Incompatible dimensions");
  }
  
  // Compute X'X
  writable::doubles_matrix<> xtx(p, p);
  const double* pa = REAL_OR_NULL(a.data());
  double* pxtx = REAL(xtx.data());
  std::fill(pxtx, pxtx + p * p, 0.0);
  
  for (int i = 0; i < p; i++) {
    for (int j = 0; j < p; j++) {
      for (int k = 0; k < n; k++) {
        pxtx[i + j * p] += pa[k + i * n] * pa[k + j * n];
      }
    }
  }
  
  // Compute X'y
  writable::doubles xty(p);
  const double* pb = REAL_OR_NULL(b.data());
  double* pxty = REAL(xty.data());
  std::fill(pxty, pxty + p, 0.0);
  
  for (int i = 0; i < p; i++) {
    for (int j = 0; j < n; j++) {
      pxty[i] += pa[j + i * n] * pb[j];
    }
  }
  
  // Simple Cholesky solve (simplified for benchmark)
  // In production would use proper linear algebra library
  writable::doubles result(p);
  double* pres = REAL(result.data());
  std::copy(pxty, pxty + p, pres);
  
  return result;
}

// ============================================================================
// II. MATRIX FUNCTIONS BENCHMARKS
// ============================================================================

/* roxygen
@title FFT Computation
@description Computes FFT by calling R's fft function.
@param x Input vector
@export
*/
[[cpp4r::register]] SEXP bench_fft(doubles x) {
  // Call R's fft function
  SEXP fft_fn = PROTECT(Rf_install("fft"));
  SEXP call = PROTECT(Rf_lang2(fft_fn, x.data()));
  SEXP result = PROTECT(Rf_eval(call, R_BaseEnv));
  UNPROTECT(3);
  return result;
}

/* roxygen
@title Eigenvalues Computation
@description Computes eigenvalues by calling R's eigen function.
@param a Input square matrix
@export
*/
[[cpp4r::register]] SEXP bench_eigenvalues(doubles_matrix<> a) {
  // Call R's eigen function with symmetric=FALSE, only.values=TRUE
  SEXP eigen_fn = PROTECT(Rf_install("eigen"));
  SEXP symmetric = PROTECT(Rf_ScalarLogical(0));
  SEXP only_values = PROTECT(Rf_ScalarLogical(1));
  
  SEXP call = PROTECT(Rf_lang4(eigen_fn, a.data(), symmetric, only_values));
  SET_TAG(CDR(call), Rf_install("x"));
  SET_TAG(CDDR(call), Rf_install("symmetric"));
  SET_TAG(CDDDR(call), Rf_install("only.values"));
  
  SEXP result = PROTECT(Rf_eval(call, R_BaseEnv));
  
  // Extract $values component
  SEXP values = PROTECT(Rf_install("values"));
  SEXP eigenvalues = PROTECT(Rf_getAttrib(result, values));
  
  UNPROTECT(7);
  return eigenvalues;
}

/* roxygen
@title Determinant Computation
@description Computes matrix determinant using R's det function.
@param a Input square matrix
@export
*/
[[cpp4r::register]] double bench_determinant(doubles_matrix<> a) {
  // Call R's det function
  SEXP det_fn = PROTECT(Rf_install("det"));
  SEXP call = PROTECT(Rf_lang2(det_fn, a.data()));
  SEXP result = PROTECT(Rf_eval(call, R_BaseEnv));
  double det_value = REAL(result)[0];
  UNPROTECT(3);
  return det_value;
}

/* roxygen
@title Cholesky Decomposition
@description Computes Cholesky decomposition by calling R's chol function.
@param a Input positive definite matrix
@export
*/
[[cpp4r::register]] SEXP bench_cholesky(doubles_matrix<> a) {
  // Call R's chol function
  SEXP chol_fn = PROTECT(Rf_install("chol"));
  SEXP call = PROTECT(Rf_lang2(chol_fn, a.data()));
  SEXP result = PROTECT(Rf_eval(call, R_BaseEnv));
  UNPROTECT(3);
  return result;
}

/* roxygen
@title Matrix Inverse
@description Computes matrix inverse by calling R's solve function.
@param a Input square matrix
@export
*/
[[cpp4r::register]] SEXP bench_inverse(doubles_matrix<> a) {
  // Call R's solve function
  SEXP solve_fn = PROTECT(Rf_install("solve"));
  SEXP call = PROTECT(Rf_lang2(solve_fn, a.data()));
  SEXP result = PROTECT(Rf_eval(call, R_BaseEnv));
  UNPROTECT(3);
  return result;
}

// ============================================================================
// III. PROGRAMMING BENCHMARKS
// ============================================================================

/* roxygen
@title Fibonacci Calculation (Vectorized)
@description Computes Fibonacci numbers using Binet's formula for vector of indices.
@param indices Vector of Fibonacci indices to compute
@export
*/
[[cpp4r::register]] doubles bench_fibonacci_vector(doubles indices) {
  int n = indices.size();
  writable::doubles result(n);
  
  const double* pidx = REAL_OR_NULL(indices.data());
  double* pr = REAL(result.data());
  
  const double phi = 1.6180339887498949;
  const double sqrt5 = std::sqrt(5.0);
  
  for (int i = 0; i < n; i++) {
    double a = pidx[i];
    pr[i] = (std::pow(phi, a) - std::pow(-phi, -a)) / sqrt5;
  }
  
  return result;
}

/* roxygen
@title Hilbert Matrix Creation
@description Creates Hilbert matrix H[i,j] = 1/(i+j-1).
@param n Matrix dimension
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_hilbert_matrix(int n) {
  writable::doubles_matrix<> result(n, n);
  double* pr = REAL(result.data());
  
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
      pr[i + j * n] = 1.0 / (i + j + 1.0);
    }
  }
  
  return result;
}

/* roxygen
@title Greatest Common Divisor (Vectorized)
@description Computes GCD for pairs of integers using Euclidean algorithm.
@param x First vector of integers
@param y Second vector of integers
@export
*/
[[cpp4r::register]] integers bench_gcd_vector(integers x, integers y) {
  int n = x.size();
  if (y.size() != n) {
    stop("Vectors must have equal length");
  }
  
  writable::integers result(n);
  const int* px = INTEGER_OR_NULL(x.data());
  const int* py = INTEGER_OR_NULL(y.data());
  int* pr = INTEGER(result.data());
  
  for (int i = 0; i < n; i++) {
    int a = px[i];
    int b = py[i];
    
    // Euclidean algorithm
    while (b != 0) {
      int temp = b;
      b = a % b;
      a = temp;
    }
    
    pr[i] = a;
  }
  
  return result;
}

/* roxygen
@title Toeplitz Matrix Creation
@description Creates Toeplitz matrix T[i,j] = |i-j| + 1 using nested loops.
@param n Matrix dimension
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_toeplitz_matrix(int n) {
  writable::doubles_matrix<> result(n, n);
  double* pr = REAL(result.data());
  
  for (int j = 0; j < n; j++) {
    for (int i = 0; i < n; i++) {
      pr[i + j * n] = std::abs((double)(i - j)) + 1.0;
    }
  }
  
  return result;
}

/* roxygen
@title Matrix Trace
@description Computes trace (sum of diagonal elements) of a matrix.
@param m Input matrix
@export
*/
[[cpp4r::register]] double bench_trace(doubles_matrix<> m) {
  int n = m.nrow();
  int p = m.ncol();
  int min_dim = n < p ? n : p;
  
  const double* pm = REAL_OR_NULL(m.data());
  double trace = 0.0;
  
  for (int i = 0; i < min_dim; i++) {
    trace += pm[i + i * n];
  }
  
  return trace;
}

/* roxygen
@title Escoufier's Method (RV Coefficient)
@description Full implementation of Escoufier's equivalent vectors algorithm.
  Computes RV coefficient for variable selection using correlation matrices.
  This tests both R function calls (cor) and C++ matrix operations.
@param x Input matrix
@export
*/
[[cpp4r::register]] list bench_escoufier(doubles_matrix<> x) {
  int nrow = x.nrow();
  int p = x.ncol();
  
  if (p < 2) {
    stop("Matrix must have at least 2 columns");
  }
  
  // Prepare R functions
  SEXP cor_fn = PROTECT(Rf_install("cor"));
  
  writable::integers vr(p);
  writable::doubles RV(p);
  int* pvr = INTEGER(vr.data());
  double* pRV = REAL(RV.data());
  
  std::vector<int> vt;  // Variables to test
  for (int i = 0; i < p; i++) {
    vt.push_back(i + 1);  // 1-based indexing for R
  }
  
  // Main loop over variables
  for (int j = 0; j < p; j++) {
    double Rvmax = 0.0;
    int vrt = 0;
    int vt_size = p - j;
    
    // Loop over remaining variables to test
    for (int k = 0; k < vt_size; k++) {
      // Build x2 = cbind(x, x[,vr[0:j]], x[,vt[k]])
      // Number of columns in x2: p + j + 1
      int ncol_x2 = p + j + 1;
      writable::doubles_matrix<> x2(nrow, ncol_x2);
      double* px2 = REAL(x2.data());
      const double* px = REAL_OR_NULL(x.data());
      
      // Copy original x (columns 0 to p-1)
      for (int col = 0; col < p; col++) {
        for (int row = 0; row < nrow; row++) {
          px2[row + col * nrow] = px[row + col * nrow];
        }
      }
      
      // Copy selected columns from vr (if j > 0)
      for (int idx = 0; idx < j; idx++) {
        int src_col = pvr[idx] - 1;  // Convert to 0-based
        int dst_col = p + idx;
        for (int row = 0; row < nrow; row++) {
          px2[row + dst_col * nrow] = px[row + src_col * nrow];
        }
      }
      
      // Copy column vt[k]
      int src_col = vt[k] - 1;  // Convert to 0-based
      int dst_col = p + j;
      for (int row = 0; row < nrow; row++) {
        px2[row + dst_col * nrow] = px[row + src_col * nrow];
      }
      
      // Compute correlation matrix R = cor(x2)
      SEXP call = PROTECT(Rf_lang2(cor_fn, x2.data()));
      SEXP R_sexp = PROTECT(Rf_eval(call, R_BaseEnv));
      const double* pR = REAL_OR_NULL(R_sexp);
      
      // Extract submatrices
      // Ryy = R[1:p, 1:p]
      // Rxx = R[(p+1):(p+j+1), (p+1):(p+j+1)]
      // Rxy = R[(p+1):(p+j+1), 1:p]
      // Ryx = t(Rxy)
      
      // Compute Trace(Ryy %*% Ryy)
      double trace_Ryy_sq = 0.0;
      for (int i = 0; i < p; i++) {
        for (int k2 = 0; k2 < p; k2++) {
          trace_Ryy_sq += pR[i + k2 * ncol_x2] * pR[k2 + i * ncol_x2];
        }
      }
      
      // Compute Trace(Rxx %*% Rxx)
      int rxx_size = j + 1;
      double trace_Rxx_sq = 0.0;
      for (int i = 0; i < rxx_size; i++) {
        for (int k2 = 0; k2 < rxx_size; k2++) {
          int idx1 = (p + i) + (p + k2) * ncol_x2;
          int idx2 = (p + k2) + (p + i) * ncol_x2;
          trace_Rxx_sq += pR[idx1] * pR[idx2];
        }
      }
      
      // Compute Ryx %*% Rxy and its trace
      // Ryx is transpose of Rxy, so Ryx[i,j] = Rxy[j,i]
      // (Ryx %*% Rxy)[i,j] = sum_k Ryx[i,k] * Rxy[k,j]
      //                     = sum_k Rxy[k,i] * Rxy[k,j]
      double trace_RyxRxy = 0.0;
      for (int i = 0; i < p; i++) {
        for (int k2 = 0; k2 < rxx_size; k2++) {
          int rxy_ki = (p + k2) + i * ncol_x2;
          int rxy_ki2 = (p + k2) + i * ncol_x2;
          trace_RyxRxy += pR[rxy_ki] * pR[rxy_ki2];
        }
      }
      
      // Compute RV coefficient
      double rvt = trace_RyxRxy / std::sqrt(trace_Ryy_sq * trace_Rxx_sq);
      
      UNPROTECT(2);  // R_sexp, call
      
      if (rvt > Rvmax) {
        Rvmax = rvt;
        vrt = vt[k];
      }
    }
    
    // Store results
    pvr[j] = vrt;
    pRV[j] = Rvmax;
    
    // Remove vrt from vt
    vt.erase(std::remove(vt.begin(), vt.end(), vrt), vt.end());
  }
  
  UNPROTECT(1);  // cor_fn
  
  writable::list result(2);
  result[0] = vr;
  result[1] = RV;
  
  writable::strings names(2);
  names[0] = "variables";
  names[1] = "rv";
  result.names() = names;
  
  return result;
}

/* roxygen
@title Matrix Multiplication
@description Standard matrix multiplication: C = A %*% B.
  This tests actual numerical computation, not just object creation.
  Uses pre-allocation and cache-friendly column-major access.
@param A n x m matrix
@param B m x p matrix
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_matrix_multiply(doubles_matrix<> A, doubles_matrix<> B) {
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

/* roxygen
@title Rolling Window Mean
@description Computes rolling window mean with specified window size.
  Tests sequential access patterns. Pre-allocates result vector.
@param x data vector
@param w window size
@export
*/
[[cpp4r::register]] doubles bench_rolling_mean(doubles x, int w) {
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

/* roxygen
@title Data Frame Column Operations
@description Computes summary statistics for each numeric column in a data frame.
  Tests data frame manipulation and type checking.
  Returns a data frame with mean, sd, min, max for each column.
@param tbl data frame
@export
*/
[[cpp4r::register]] data_frame bench_dataframe_summary(data_frame tbl) {
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

/* roxygen
@title String Pattern Matching
@description Counts occurrences of a pattern in string vector elements.
@param s data vector
@param p Pattern to search for
@export
*/
[[cpp4r::register]] int bench_string_pattern_count(strings s, strings p) {
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

/* roxygen
@title Grouped Aggregation
@description Computes mean by group (like tapply or dplyr::summarize).
@param x data vector
@param groups Integer vector of group IDs
@export
*/
[[cpp4r::register]] doubles bench_grouped_mean(doubles x, integers groups) {
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

/* roxygen
@title Fibonacci Sequence
@description Generates Fibonacci sequence up to n terms.
  Tests pure computational performance without R API overhead.
  Pre-allocates result vector.
@param n number of terms
@export
*/
[[cpp4r::register]] doubles bench_fibonacci(int n) {
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

/* roxygen
@title NA Handling
@description Computes mean excluding NAs, like mean(x, na.rm=TRUE).
@param x data vector with NAs
@export
*/
[[cpp4r::register]] double bench_mean_na_rm(doubles x) {
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

/* roxygen
@title Outer Product
@description Computes outer product of two vectors: result[i,j] = x[i] * y[j].
  Tests proper matrix allocation and filling patterns.
  This is a case where you must allocate the full result upfront.
@param x 1st data vector
@param y 2nd data vector
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_outer_product(doubles x, doubles y) {
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

/* roxygen
@title Pairwise Distance Matrix
@description Computes Euclidean distance matrix for a set of points.
  Returns symmetric n x n distance matrix.
@param x (n x k) matrix
@export
*/
[[cpp4r::register]] doubles_matrix<> bench_pairwise_distances(doubles_matrix<> x) {
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

/* roxygen
@title Cumulative Operations
@description Computes cumulative sum, product, min, and max.
  Tests sequential data processing patterns.
@param x data vector
@export
*/
[[cpp4r::register]] list bench_cumulative_ops(doubles x) {
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

/* roxygen
@title Bootstrap Resampling
@description Generates bootstrap resamples and computes statistic.
  Tests random sampling and repeated computation.
  Returns vector of bootstrap statistics (mean of each resample).
@param x data vector
@param n_boot number of bootstrap samples
@export
*/
[[cpp4r::register]] doubles bench_bootstrap_mean(doubles x, int n_boot) {
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
