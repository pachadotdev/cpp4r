
/* roxygen
@title Gibbs Sampler in C++
@description Test suite
@param N number of iterations
@param thin thinning parameter
@export
*/
[[cpp4r::register]] SEXP gibbs_cpp_(int N, int thin) {
  cpp4r::writable::doubles_matrix<> mat(N, 2);
  double x = 0, y = 0;
  GetRNGstate();
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < thin; j++) {
      x = Rf_rgamma(3., 1. / double(y * y + 4));
      y = Rf_rnorm(1. / (x + 1.), 1. / sqrt(2. * (x + 1.)));
      // REprintf("x: %f y: %f\n", x, y);
    }
    mat(i, 0) = x;
    mat(i, 1) = y;
  }
  PutRNGstate();
  return mat;
}

/* roxygen
@title Gibbs Sampler in C++ (Alternative)
@description Test suite
@param N number of iterations
@param thin thinning parameter
@export
*/
[[cpp4r::register]] cpp4r::doubles_matrix<> gibbs_cpp2_(int N, int thin) {
  cpp4r::writable::doubles_matrix<> mat(N, 2);
  double x = 0, y = 0;
  GetRNGstate();
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < thin; j++) {
      x = Rf_rgamma(3., 1. / double(y * y + 4));
      y = Rf_rnorm(1. / (x + 1.), 1. / sqrt(2. * (x + 1.)));
    }
    mat(i, 0) = x;
    mat(i, 1) = y;
  }
  PutRNGstate();
  return mat;
}

/* roxygen
@title Convert Raw Vector to Raw Vector (Identity)
@description Test suite
@param x vector of raw bytes (R)
@export
*/
[[cpp4r::register]] cpp4r::doubles row_sums_(cpp4r::doubles_matrix<cpp4r::by_row> x) {
  cpp4r::writable::doubles sums(x.nrow());

  int i = 0;
  for (auto row : x) {
    sums[i] = 0.;
    for (auto&& val : row) {
      if (cpp4r::is_na(val)) {
        sums[i] = NA_REAL;
        break;
      }
      sums[i] += val;
    }
    ++i;
  }

  return sums;
}

/* roxygen
@title Copy Matrix with Dimnames
@description Test suite
@param x matrix to copy
@export
*/
[[cpp4r::register]] cpp4r::doubles_matrix<> mat_mat_copy_dimnames_(
    cpp4r::doubles_matrix<> x) {
  cpp4r::writable::doubles_matrix<> out = x;

  out.attr("dimnames") = x.attr("dimnames");

  return out;
}

/* roxygen
@title Copy Matrix with Dimnames (SEXP)
@description Test suite
@param x matrix to copy
@export
*/
[[cpp4r::register]] SEXP mat_sexp_copy_dimnames_(cpp4r::doubles_matrix<> x) {
  cpp4r::writable::doubles_matrix<> out = x;

  out.attr("dimnames") = x.attr("dimnames");

  return out;
}

/* roxygen
@title Create Matrix with Dimnames
@description Test suite
@export
*/
[[cpp4r::register]] cpp4r::doubles_matrix<> mat_mat_create_dimnames_() {
  cpp4r::writable::doubles_matrix<> out(2, 2);

  out(0, 0) = 1;
  out(0, 1) = 2;
  out(1, 0) = 3;
  out(1, 1) = 4;

  cpp4r::writable::list dimnames(2);
  dimnames[0] = cpp4r::strings({"a", "b"});
  dimnames[1] = cpp4r::strings({"c", "d"});

  out.attr("dimnames") = dimnames;

  return out;
}

/* roxygen
@title Compute Column Sums
@description Test suite
@param x matrix of doubles (R)
@export
*/
[[cpp4r::register]] cpp4r::doubles col_sums_(cpp4r::doubles_matrix<cpp4r::by_column> x) {
  cpp4r::writable::doubles sums(x.ncol());

  int i = 0;
  for (auto col : x) {
    sums[i] = 0.;
    for (auto&& val : col) {
      if (cpp4r::is_na(val)) {
        sums[i] = NA_REAL;
        break;
      }
      sums[i] += val;
    }
    ++i;
  }

  return sums;
}

// Test function for automatic integer to double matrix coercion
/* roxygen
@title Add Two Matrices (with Coercion)
@description Test suite
@param x first matrix (R)
@param y second matrix (R)
@export
*/
[[cpp4r::register]] cpp4r::doubles_matrix<> matrix_add_(
    const cpp4r::doubles_matrix<>& x, const cpp4r::doubles_matrix<>& y) {
  int nrow = x.nrow();
  int ncol = x.ncol();

  cpp4r::writable::doubles_matrix<> result(nrow, ncol);

  for (int i = 0; i < nrow; i++) {
    for (int j = 0; j < ncol; j++) {
      result(i, j) = x(i, j) + y(i, j);
    }
  }

  return result;
}

// Test function specifically for integer matrix coercion
// This takes doubles_matrix<> but should accept integer matrices via implicit coercion
/* roxygen
@title Add Two Matrices (with Integer Coercion)
@description Test suite
@param x first matrix (R)
@param y second matrix (R)
@export
*/
[[cpp4r::register]] cpp4r::doubles_matrix<> matrix_add_coerce_test_(
    const cpp4r::doubles_matrix<>& x, const cpp4r::doubles_matrix<>& y) {
  int nrow = x.nrow();
  int ncol = x.ncol();

  cpp4r::writable::doubles_matrix<> result(nrow, ncol);

  for (int i = 0; i < nrow; i++) {
    for (int j = 0; j < ncol; j++) {
      result(i, j) = x(i, j) + y(i, j);
    }
  }

  return result;
}

// Test function for integer + double mixed coercion
/* roxygen
@title Add Integer Matrix and Double Matrix
@description Test suite
@param int_mat integer matrix (R)
@param dbl_mat double matrix (R)
@export
*/
[[cpp4r::register]] cpp4r::doubles_matrix<> matrix_mixed_add_(
    const cpp4r::doubles_matrix<>& int_mat, const cpp4r::doubles_matrix<>& dbl_mat) {
  int nrow = int_mat.nrow();
  int ncol = int_mat.ncol();

  cpp4r::writable::doubles_matrix<> result(nrow, ncol);

  for (int i = 0; i < nrow; i++) {
    for (int j = 0; j < ncol; j++) {
      result(i, j) = int_mat(i, j) + dbl_mat(i, j);
    }
  }

  return result;
}
