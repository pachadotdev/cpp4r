[[cpp4r::register]] SEXP gibbs_cpp(int N, int thin) {
  cpp4r::writable::doubles_matrix<> mat(N, 2);
  double x = 0, y = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < thin; j++) {
      x = Rf_rgamma(3., 1. / double(y * y + 4));
      y = Rf_rnorm(1. / (x + 1.), 1. / sqrt(2. * (x + 1.)));
      // REprintf("x: %f y: %f\n", x, y);
    }
    mat[i][0] = x;
    mat[i][1] = y;
  }
  return mat;
}

[[cpp4r::register]] cpp4r::doubles_matrix<> gibbs_cpp2(int N, int thin) {
  cpp4r::writable::doubles_matrix<> mat(N, 2);
  double x = 0, y = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < thin; j++) {
      x = Rf_rgamma(3., 1. / double(y * y + 4));
      y = Rf_rnorm(1. / (x + 1.), 1. / sqrt(2. * (x + 1.)));
    }
    mat(i, 0) = x;
    mat(i, 1) = y;
  }
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
