#pragma once
#include <cpp4r.hpp>
using namespace cpp4r;

[[cpp4r::register]]
list sexp_list_init_() {
  using namespace cpp4r::literals;
  writable::list out({
    "int"_nm = {1, 2, 3},
    "dbl"_nm = {1., 2., 3.},
    "char"_nm = {"x", "y", "z"},
  });
  out.attr("class") = "data.frame";
  out.attr("row.names") = {NA_INTEGER, -3};
  return out;
}

[[cpp4r::register]]
list sexp_scalar_list_init_() {
  using namespace cpp4r::literals;
  writable::list out({
    "int"_nm = 1,
    "dbl"_nm = 1.,
    "char"_nm = "x",
  });
  out.attr("class") = "data.frame";
  out.attr("row.names") = {NA_INTEGER, -1};
  return out;
}
