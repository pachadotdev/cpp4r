#include <omp.h>
#include <cpp4r.hpp>

using namespace cpp4r;

[[cpp4r::register]] list squared_unnamed_(doubles x) {
  // create vectors y = x^2 and z = thread number
  int n = x.size();
  writable::doubles y(n);
  writable::doubles z(n);
#pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    y[i] = x[i] * x[i];
    z[i] = omp_get_thread_num();
  }

  // create a list containing y and z
  writable::list out;
  out.push_back(y);
  out.push_back(z);
  return out;
}

[[cpp4r::register]] list squared_named_(doubles x) {
  // create vectors y = x^2 and z = thread number
  int n = x.size();
  writable::doubles y(n);
  writable::doubles z(n);
#pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    y[i] = x[i] * x[i];
    z[i] = omp_get_thread_num();
  }

  // create a list containing y and z
  writable::list out;
  out.push_back({"x^2"_nm = y});
  out.push_back({"thread"_nm = z});
  return out;
}
