[[cpp4r::register]] SEXP gibbs_cpp(int N, int thin) {
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

[[cpp4r::register]] cpp4r::doubles_matrix<> gibbs_cpp2(int N, int thin) {
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

[[cpp4r::register]] cpp4r::doubles row_sums(cpp4r::doubles_matrix<cpp4r::by_row> x) {
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

[[cpp4r::register]] cpp4r::doubles_matrix<> mat_mat_copy_dimnames(
    cpp4r::doubles_matrix<> x) {
  cpp4r::writable::doubles_matrix<> out = x;

  out.attr("dimnames") = x.attr("dimnames");

  return out;
}

[[cpp4r::register]] SEXP mat_sexp_copy_dimnames(cpp4r::doubles_matrix<> x) {
  cpp4r::writable::doubles_matrix<> out = x;

  out.attr("dimnames") = x.attr("dimnames");

  return out;
}

[[cpp4r::register]] cpp4r::doubles_matrix<> mat_mat_create_dimnames() {
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

[[cpp4r::register]] cpp4r::doubles col_sums(cpp4r::doubles_matrix<cpp4r::by_column> x) {
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
[[cpp4r::register]] cpp4r::doubles_matrix<> matrix_add(const cpp4r::doubles_matrix<>& x,
                                                       const cpp4r::doubles_matrix<>& y) {
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
[[cpp4r::register]] cpp4r::doubles_matrix<>
matrix_add_coerce_test(const cpp4r::doubles_matrix<>& x,
                       const cpp4r::doubles_matrix<>& y) {
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
[[cpp4r::register]] cpp4r::doubles_matrix<>
matrix_mixed_add(const cpp4r::doubles_matrix<>& int_mat,
                 const cpp4r::doubles_matrix<>& dbl_mat) {
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
