#include <cpp4r/doubles.hpp>
#include <cpp4r/matrix.hpp>

using namespace cpp4r;

[[cpp4r::register]] doubles_matrix<> invert_matrix_(doubles_matrix<> A) {
  // Obtain (X'X)^-1 via Gauss-Jordan

  // Check dimensions

  int N = A.nrow();
  int M = A.ncol();

  if (N != M) {
    stop("X must be a square matrix");
  }

  // Copy the matrix

  writable::doubles_matrix<> Acopy(N, N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      Acopy(i, j) = A(i, j);
    }
  }

  // Create the identity matrix as a starting point for Gauss-Jordan

  writable::doubles_matrix<> Ainv(N, N);

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (i == j) {
        Ainv(i, j) = 1.0;
      } else {
        Ainv(i, j) = 0.0;
      }
    }
  }

  // Overwrite Ainv by steps with the inverse of A
  // in other words, find the echelon form of A

  for (int i = 0; i < M; i++) {
    double a = Acopy(i, i);

    for (int j = 0; j < M; j++) {
      Acopy(i, j) /= a;
      Ainv(i, j) /= a;
    }

    for (int j = 0; j < M; j++) {
      if (i != j) {
        a = Acopy(j, i);

        for (int k = 0; k < M; k++) {
          Acopy(j, k) -= Acopy(i, k) * a;
          Ainv(j, k) -= Ainv(i, k) * a;
        }
      }
    }
  }

  return Ainv;
}

[[cpp4r::register]] doubles_matrix<> solve_system_(doubles_matrix<> A,
                                                   doubles_matrix<> b) {
  // Use Gauss-Jordan to solve the system Ax = b

  // Check dimensions

  int N1 = A.nrow();
  int M1 = A.ncol();

  if (N1 != M1) {
    stop("A must be a square matrix");
  }

  int N2 = b.nrow();
  int M2 = b.ncol();

  if (N1 != N2) {
    stop("b must have the same number of rows as A");
  }

  if (M2 != 1) {
    stop("b must be a column vector");
  }

  // Obtain the inverse

  // writable::doubles_matrix<> Acopy(N1,N1);

  // for (int i = 0; i < N1; i++)
  // {
  //     for (int j = 0; j < N1; j++)
  //     {
  //         Acopy(i, j) = A(i, j);
  //     }
  // }

  // doubles_matrix<> Ainv = invert_matrix_(Acopy);

  // I don´t need to create a copy in this case

  doubles_matrix<> Ainv = invert_matrix_(A);

  // Multiply Ainv by b

  writable::doubles_matrix<> x(N1, 1);

  for (int i = 0; i < N1; i++) {
    x(i, 0) = 0.0;

    for (int j = 0; j < N1; j++) {
      x(i, 0) += Ainv(i, j) * b(j, 0);
    }
  }

  return x;
}
