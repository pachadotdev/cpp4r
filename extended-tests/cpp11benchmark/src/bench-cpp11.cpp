// using optimization hints for a fair and direct comparison between cpp11-cpp4r-Rcpp

#include <cpp11.hpp>
#include <cmath>
#include <cstring>
#include <vector>

using namespace cpp11;

inline bool rrc_(std::vector<int>& excluded, const double* XtX_ptr,
                 size_t p, double tol) {
  excluded.assign(p, 0);

  if (p == 0) return true;

  std::vector<double> R(p * p, 0.0);
  double* R_ptr = R.data();
  int* excluded_ptr = excluded.data();

  size_t n_excluded = 0;

  for (size_t j = 0; j < p; ++j) {
    double R_jj = XtX_ptr[j + j * p];

    if (j > 0) {
      const double* R_j_ptr = R_ptr + j * p;
      for (size_t k = 0; k < j; ++k) {
        if (excluded_ptr[k] == 0) {
          double R_jk = R_j_ptr[k];
          R_jj -= R_jk * R_jk;
        }
      }
    }

    if (R_jj < tol) {
      excluded_ptr[j] = 1;
      n_excluded++;
      continue;
    }

    R_jj = std::sqrt(R_jj);
    R_ptr[j + j * p] = R_jj;
    const double inv_R_jj = 1.0 / R_jj;

    for (size_t col = j + 1; col < p; ++col) {
      double R_j_col = XtX_ptr[j + col * p];

      const double* R_col_ptr = R_ptr + col * p;
      const double* R_j_ptr = R_ptr + j * p;

      for (size_t k = 0; k < j; ++k) {
        if (excluded_ptr[k] == 0) {
          R_j_col -= R_j_ptr[k] * R_col_ptr[k];
        }
      }

      R_ptr[j + col * p] = R_j_col * inv_R_jj;
    }
  }

  return n_excluded < p;
}

[[cpp11::register]] list rrc_cpp11_(const doubles_matrix<>& xtx, double tol) {
  size_t p = static_cast<size_t>(xtx.ncol());
  const double* XtX_ptr = REAL(xtx.data());

  std::vector<int> excluded;
  bool success = rrc_(excluded, XtX_ptr, p, tol);

  writable::integers excluded_r(static_cast<R_xlen_t>(p));
  for (size_t i = 0; i < p; ++i) {
    excluded_r[static_cast<R_xlen_t>(i)] = excluded[i];
  }

  writable::list result;
  result.push_back({"excluded"_nm = excluded_r});
  result.push_back({"success"_nm = success});

  return result;
}
