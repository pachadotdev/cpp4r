// Helper functions for list_of and complex operations

#pragma once

#include <cpp4r.hpp>

using namespace cpp4r;

/* roxygen
@title Create List of Doubles
@description Test suite
@export
*/
[[cpp4r::register]] list_of<doubles> list_of_doubles_() {
  writable::list_of<writable::doubles> result(3);

  writable::doubles v1({1.0, 2.0});
  writable::doubles v2({3.0, 4.0, 5.0});
  writable::doubles v3({6.0});

  result[0] = v1;
  result[1] = v2;
  result[2] = v3;

  return result;
}

/* roxygen
@title Create List of Integers
@description Test suite
@export
*/
[[cpp4r::register]] list_of<integers> list_of_integers_() {
  writable::list_of<writable::integers> result(2);

  writable::integers v1({1, 2, 3});
  writable::integers v2({4, 5});

  result[0] = v1;
  result[1] = v2;

  return result;
}

/* roxygen
@title Create List of Strings
@description Test suite
@export
*/
[[cpp4r::register]] list_of<strings> list_of_strings_() {
  writable::list_of<writable::strings> result(2);

  writable::strings v1({"hello", "world"});
  writable::strings v2({"foo", "bar", "baz"});

  result[0] = v1;
  result[1] = v2;

  return result;
}

/* roxygen
@title Create Named List
@description Test suite
@export
*/
[[cpp4r::register]] writable::list list_of_named_() {
  using namespace cpp4r::literals;

  writable::doubles v1({1.0, 2.0});
  writable::doubles v2({3.0, 4.0});

  return writable::list({"a"_nm = static_cast<SEXP>(v1), "b"_nm = static_cast<SEXP>(v2)});
}

/* roxygen
@title Create Complex Numbers on 'C++' Side (typed in, r_complex out)
@description Test suite
@param real vector of real parts
@param imag vector of imaginary parts
@export
*/
[[cpp4r::register]]
writable::complexes make_complex_(doubles real, doubles imag) {
  R_xlen_t n = real.size();
  writable::complexes result(n);

  for (R_xlen_t i = 0; i < n; ++i) {
    result[i] = r_complex(real[i], imag[i]);
  }

  return result;
}

/* roxygen
@title Extract Real Part of Complex Numbers on 'C++' Side (typed in, r_complex out)
@description Test suite
@param x vector of complex numbers
@export
*/
[[cpp4r::register]] writable::doubles complex_real_(complexes x) {
  writable::doubles result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    result[i] = x[i].real();
  }
  return result;
}

/* roxygen
@title Extract Imaginary Part of Complex Numbers on 'C++' Side (typed in, r_complex out)
@description Test suite
@param x vector of complex numbers
@export
*/
[[cpp4r::register]] writable::doubles complex_imag_(complexes x) {
  writable::doubles result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    result[i] = x[i].imag();
  }
  return result;
}

/* roxygen
@title Compute Modulus of Complex Numbers on 'C++' Side (typed in, r_complex out)
@description Test suite
@param x vector of complex numbers
@export
*/
[[cpp4r::register]] writable::doubles complex_modulus_(complexes x) {
  writable::doubles result(x.size());
  for (R_xlen_t i = 0; i < x.size(); ++i) {
    double re = x[i].real();
    double im = x[i].imag();
    result[i] = std::sqrt(re * re + im * im);
  }
  return result;
}

/* roxygen
@title Add Complex Vectors on 'C++' Side (typed in, complexes out)
@description Test suite
@param x vector of complex numbers
@param y vector of complex numbers
@export
*/
[[cpp4r::register]]
writable::complexes complex_add_(complexes x, complexes y) {
  R_xlen_t n = x.size();
  writable::complexes result(n);

  for (R_xlen_t i = 0; i < n; ++i) {
    result[i] = r_complex(x[i].real() + y[i].real(), x[i].imag() + y[i].imag());
  }

  return result;
}
