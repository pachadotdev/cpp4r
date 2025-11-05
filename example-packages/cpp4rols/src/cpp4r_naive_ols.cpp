#include <cpp4r.hpp>

using namespace cpp4r;

doubles_matrix<> XtX(doubles_matrix<> X) {
  int NX = X.nrow();
  int MX = X.ncol();

  writable::doubles_matrix<> R(MX, MX);

  for (int i = 0; i < MX; i++) {
    for (int j = 0; j < MX; j++) {
      for (int k = 0; k < NX; k++) {
        R(i, j) += X(k, i) * X(k, j);
      }
    }
  }

  return R;
}

doubles_matrix<> XtXinv(doubles_matrix<> X) {
  // Obtain (X'X)^-1 via Gauss-Jordan

  writable::doubles_matrix<> A = XtX(X);

  int MX = A.nrow();

  // Create the identity matrix as a starting point for Gauss-Jordan

  writable::doubles_matrix<> Ainv(MX, MX);

  for (int i = 0; i < MX; i++) {
    for (int j = 0; j < MX; j++) {
      if (i == j) {
        Ainv(i, j) = 1.0;
      } else {
        Ainv(i, j) = 0.0;
      }
    }
  }

  // Overwrite Ainv by steps with the inverse of A
  // in other words, find the echelon form of A

  for (int i = 0; i < MX; i++) {
    double a = A(i, i);

    for (int j = 0; j < MX; j++) {
      A(i, j) /= a;
      Ainv(i, j) /= a;
    }

    for (int j = 0; j < MX; j++) {
      if (i != j) {
        a = A(j, i);

        for (int k = 0; k < MX; k++) {
          A(j, k) -= A(i, k) * a;
          Ainv(j, k) -= Ainv(i, k) * a;
        }
      }
    }
  }

  return Ainv;
}

doubles_matrix<> Xt(doubles_matrix<> X) {
  int NX = X.nrow();
  int MX = X.ncol();

  writable::doubles_matrix<> R(MX, NX);

  for (int i = 0; i < MX; i++) {
    for (int j = 0; j < NX; j++) {
      R(i, j) = X(j, i);
    }
  }

  return R;
}

doubles_matrix<> XtY(doubles_matrix<> X, doubles_matrix<> Y) {
  int NX = X.nrow();
  int MX = X.ncol();

  doubles_matrix<> A = Xt(X);
  writable::doubles_matrix<> B(MX, 1);

  for (int i = 0; i < MX; i++) {
    for (int j = 0; j < NX; j++) {
      B(i, 0) += A(i, j) * Y(j, 0);
    }
  }

  return B;
}

doubles_matrix<> XtXinvXtY(doubles_matrix<> XtXinv, doubles_matrix<> XtY) {
  int MX = XtXinv.nrow();

  writable::doubles_matrix<> R(MX, 1);

  for (int i = 0; i < MX; i++) {
    for (int j = 0; j < MX; j++) {
      R(i, 0) += XtXinv(i, j) * XtY(j, 0);
    }
  }

  return R;
}

[[cpp4r::register]]
doubles_matrix<> naive_ols_(doubles_matrix<> X, doubles_matrix<> Y) {
  // 1. Check dimensions

  int NX = X.nrow();

  int NY = Y.nrow();
  int MY = Y.ncol();

  if (NX != NY) {
    stop("X and Y must have the same number of rows");
  }

  if (MY != 1) {
    stop("Y must have only one column");
  }

  // 2. Compute (X'X)^-1

  doubles_matrix<> A = XtXinv(X);

  // 3. Compute (X'Y)

  doubles_matrix<> B = XtY(X, Y);

  // 4. Compute (X'X)^-1(X'Y)

  doubles_matrix<> C = XtXinvXtY(A, B);

  return C;
}
