// using optimization hints for a fair and direct comparison between cpp11-cpp4r-Rcpp

#include <cpp4r.hpp>
#include <cstring>
#include <vector>

using namespace cpp4r;

/* roxygen
@title Add Two Matrices
@export
*/
[[cpp4r::register]] doubles_matrix<> add_two_cpp4r(const doubles_matrix<>& a,
                                                   const doubles_matrix<>& b) {
  int nrow = a.nrow();
  int ncol = a.ncol();

  // Create output matrix
  writable::doubles_matrix<> Z(nrow, ncol);

  // Use optimized data_ptr() methods to avoid REAL() macro overhead
  const double* __restrict__ a_ptr = a.data_ptr();
  const double* __restrict__ b_ptr = b.data_ptr();
  double* __restrict__ z_ptr = Z.data_ptr_writable();

  // Vectorized operation on flattened data
  int size = nrow * ncol;
  for (int i = 0; i < size; ++i) {
    z_ptr[i] = a_ptr[i] + b_ptr[i];
  }

  return Z;
}

/* roxygen
@title Add Four Matrices
@export
*/
[[cpp4r::register]] doubles_matrix<> add_four_cpp4r(const doubles_matrix<>& a,
                                                    const doubles_matrix<>& b,
                                                    const doubles_matrix<>& c,
                                                    const doubles_matrix<>& d) {
  int nrow = a.nrow();
  int ncol = a.ncol();

  writable::doubles_matrix<> Z(nrow, ncol);

  // Use optimized data_ptr() methods
  const double* __restrict__ a_ptr = a.data_ptr();
  const double* __restrict__ b_ptr = b.data_ptr();
  const double* __restrict__ c_ptr = c.data_ptr();
  const double* __restrict__ d_ptr = d.data_ptr();
  double* __restrict__ z_ptr = Z.data_ptr_writable();

  int size = nrow * ncol;
  for (int i = 0; i < size; ++i) {
    z_ptr[i] = a_ptr[i] + b_ptr[i] + c_ptr[i] + d_ptr[i];
  }

  return Z;
}

/* roxygen
@title Multiply Four Matrices
@export
*/
[[cpp4r::register]] doubles_matrix<> multiply_four_cpp4r(const doubles_matrix<>& a,
                                                         const doubles_matrix<>& b,
                                                         const doubles_matrix<>& c,
                                                         const doubles_matrix<>& d) {
  int n = a.ncol();
  int n5 = n / 5;
  int n10 = n / 10;
  int n15 = n / 15;
  int n20 = n / 20;

  // Use optimized data_ptr() methods
  const double* __restrict__ a_ptr = a.data_ptr();
  const double* __restrict__ b_ptr = b.data_ptr();
  const double* __restrict__ c_ptr = c.data_ptr();
  const double* __restrict__ d_ptr = d.data_ptr();

  // Step 1: A[1:n5, 1:n5] %*% B[1:n5, 1:n10]
  writable::doubles_matrix<> temp1(n5, n10);
  double* __restrict__ temp1_ptr = temp1.data_ptr_writable();

  for (int i = 0; i < n5; ++i) {
    for (int j = 0; j < n10; ++j) {
      double sum = 0.0;
      for (int k = 0; k < n5; ++k) {
        // A[i,k] where i,k < n5: a_ptr[i + k*n]
        // B[k,j] where k < n5, j < n10: b_ptr[k + j*n]
        sum += a_ptr[i + k * n] * b_ptr[k + j * n];
      }
      temp1_ptr[i + j * n5] = sum;
    }
  }

  // Step 2: temp1 %*% C[1:n10, 1:n15]
  writable::doubles_matrix<> temp2(n5, n15);
  double* __restrict__ temp2_ptr = temp2.data_ptr_writable();

  for (int i = 0; i < n5; ++i) {
    for (int j = 0; j < n15; ++j) {
      double sum = 0.0;
      for (int k = 0; k < n10; ++k) {
        // temp1[i,k]: temp1_ptr[i + k*n5]
        // C[k,j] where k < n10, j < n15: c_ptr[k + j*n]
        sum += temp1_ptr[i + k * n5] * c_ptr[k + j * n];
      }
      temp2_ptr[i + j * n5] = sum;
    }
  }

  // Step 3: temp2 %*% D[1:n15, 1:n20]
  writable::doubles_matrix<> Z(n5, n20);
  double* __restrict__ z_ptr = Z.data_ptr_writable();

  for (int i = 0; i < n5; ++i) {
    for (int j = 0; j < n20; ++j) {
      double sum = 0.0;
      for (int k = 0; k < n15; ++k) {
        // temp2[i,k]: temp2_ptr[i + k*n5]
        // D[k,j] where k < n15, j < n20: d_ptr[k + j*n]
        sum += temp2_ptr[i + k * n5] * d_ptr[k + j * n];
      }
      z_ptr[i + j * n5] = sum;
    }
  }

  return Z;
}

/* roxygen
@title Submatrix Manipulation
@export
*/
[[cpp4r::register]] doubles_matrix<> submatrix_manipulation_cpp4r(
    const doubles_matrix<>& a, const doubles_matrix<>& b) {
  int nrow = b.nrow();
  int ncol = b.ncol();

  writable::doubles_matrix<> Z(nrow, ncol);

  // Use optimized data_ptr() methods
  const double* __restrict__ a_ptr = a.data_ptr();
  const double* __restrict__ b_ptr = b.data_ptr();
  double* __restrict__ z_ptr = Z.data_ptr_writable();

  // Copy entire b matrix
  std::memcpy(z_ptr, b_ptr, nrow * ncol * sizeof(double));

  // Copy first row of a into last row of Z
  // First row of a spans: a_ptr[0], a_ptr[nrow], a_ptr[2*nrow], ..., a_ptr[(ncol-1)*nrow]
  // Last row of Z spans: z_ptr[nrow-1], z_ptr[nrow-1 + nrow], ..., z_ptr[nrow-1 +
  // (ncol-1)*nrow]
  for (int j = 0; j < ncol; ++j) {
    z_ptr[(nrow - 1) + j * nrow] = a_ptr[0 + j * a.nrow()];
  }

  return Z;
}

/* roxygen
@title Multi-Operation Expression
@export
*/
[[cpp4r::register]] double multi_operation_cpp4r(const doubles_matrix<>& a,
                                                 const doubles_matrix<>& b,
                                                 const doubles_matrix<>& c) {
  int n = a.nrow();

  // Use optimized data_ptr() methods
  const double* __restrict__ a_ptr = a.data_ptr();
  const double* __restrict__ b_ptr = b.data_ptr();
  const double* __restrict__ c_ptr = c.data_ptr();

  // Compute: t(a_col) %*% solve(diag(b_diag)) %*% c_col
  // This is equivalent to: sum(a[i,0] * (1/b[i,i]) * c[i,0]) for i in 0..n-1
  double result = 0.0;
  for (int i = 0; i < n; ++i) {
    // a[i,0] is at a_ptr[i], b[i,i] is at b_ptr[i + i*n], c[i,0] is at c_ptr[i]
    result += a_ptr[i] * (1.0 / b_ptr[i + i * n]) * c_ptr[i];
  }

  return result;
}
