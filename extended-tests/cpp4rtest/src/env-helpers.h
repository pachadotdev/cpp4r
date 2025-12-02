// Helper functions for environment and named operations

#pragma once

#include <cpp4r.hpp>

using namespace cpp4r;

// Environment operations
[[cpp4r::register]]
int cpp4r_env_get_int_(environment env, std::string name) {
  return as_cpp<int>(env[name.c_str()]);
}

[[cpp4r::register]]
std::string cpp4r_env_get_str_(environment env, std::string name) {
  return as_cpp<std::string>(env[name.c_str()]);
}

[[cpp4r::register]]
void cpp4r_env_set_(environment env, std::string name, int value) {
  env[name.c_str()] = as_sexp(value);
}

[[cpp4r::register]]
bool cpp4r_env_exists_(environment env, std::string name) {
  return env.exists(name.c_str());
}

[[cpp4r::register]]
SEXP cpp4r_global_get_(std::string name) {
  environment global(R_GlobalEnv);
  return global[name.c_str()];
}

// Named vector operations
[[cpp4r::register]]
writable::doubles cpp4r_named_doubles_() {
  using namespace cpp4r::literals;
  return writable::doubles({"a"_nm = 1.0, "b"_nm = 2.0, "c"_nm = 3.0});
}

[[cpp4r::register]]
writable::integers cpp4r_named_integers_() {
  using namespace cpp4r::literals;
  return writable::integers({"x"_nm = 10, "y"_nm = 20, "z"_nm = 30});
}

[[cpp4r::register]]
writable::strings cpp4r_named_strings_() {
  using namespace cpp4r::literals;
  return writable::strings(
      {"first"_nm = "one", "second"_nm = "two", "third"_nm = "three"});
}

[[cpp4r::register]]
writable::list cpp4r_named_list_() {
  using namespace cpp4r::literals;
  return writable::list(
      {"int"_nm = as_sexp(42), "dbl"_nm = as_sexp(3.14), "str"_nm = as_sexp("hello")});
}

[[cpp4r::register]]
double cpp4r_get_by_name_(doubles x, std::string name) {
  return x[name.c_str()];
}

[[cpp4r::register]]
bool cpp4r_contains_name_(doubles x, std::string name) {
  return x.contains(name.c_str());
}

[[cpp4r::register]]
int cpp4r_find_name_pos_(doubles x, std::string name) {
  auto it = x.find(name.c_str());
  if (it == x.end()) {
    return -1;
  }
  return static_cast<int>(std::distance(x.begin(), it)) + 1;  // 1-indexed
}
