// Helper functions for environment and named operations

#pragma once

#include <cpp4r.hpp>

using namespace cpp4r;

/* roxygen
@title Get Integer from Environment on 'C++' Side
@description Test suite
@param env R environment to query
@param name name of the variable to get
@export
*/
[[cpp4r::register]] int env_get_int_(environment env, std::string name) {
  return as_cpp<int>(env[name.c_str()]);
}

/* roxygen
@title Get String from Environment on 'C++' Side
@description Test suite
@param env R environment to query
@param name name of the variable to get
@export
*/
[[cpp4r::register]] std::string env_get_str_(environment env, std::string name) {
  return as_cpp<std::string>(env[name.c_str()]);
}

/* roxygen
@title Set Integer in Environment on 'C++' Side
@description Test suite
@param env R environment to modify
@param name name of the variable to set
@param value integer value to set
@export
*/
[[cpp4r::register]] void env_set_(environment env, std::string name, int value) {
  env[name.c_str()] = as_sexp(value);
}

/* roxygen
@title Check if Variable Exists in Environment on 'C++' Side
@description Test suite
@param env R environment to query
@param name name of the variable to check
@export
*/
[[cpp4r::register]] bool env_exists_(environment env, std::string name) {
  return env.exists(name.c_str());
}

/* roxygen
@title Get Variable from Global Environment on 'C++' Side
@description Test suite
@param name name of the variable to get
@export
*/
[[cpp4r::register]] SEXP global_get_(std::string name) {
  environment global(R_GlobalEnv);
  return global[name.c_str()];
}

/* roxygen
@title Create Named Doubles on 'C++' Side
@description Test suite
@export
*/
[[cpp4r::register]] writable::doubles named_doubles_() {
  using namespace cpp4r::literals;
  return writable::doubles({"a"_nm = 1.0, "b"_nm = 2.0, "c"_nm = 3.0});
}

/* roxygen
@title Create Named Integers on 'C++' Side
@description Test suite
@export
*/
[[cpp4r::register]] writable::integers named_integers_() {
  using namespace cpp4r::literals;
  return writable::integers({"x"_nm = 10, "y"_nm = 20, "z"_nm = 30});
}

/* roxygen
@title Create Named Strings on 'C++' Side
@description Test suite
@export
*/
[[cpp4r::register]] writable::strings named_strings_() {
  using namespace cpp4r::literals;
  return writable::strings(
      {"first"_nm = "one", "second"_nm = "two", "third"_nm = "three"});
}

/* roxygen
@title Create Named List on 'C++' Side
@description Test suite
@export
*/
[[cpp4r::register]] writable::list named_list_() {
  using namespace cpp4r::literals;
  return writable::list(
      {"int"_nm = as_sexp(42), "dbl"_nm = as_sexp(3.14), "str"_nm = as_sexp("hello")});
}

/* roxygen
@title Get Value by Name from Named Vector on 'C++' Side
@description Test suite
@param x named vector to query
@param name name of the element to get
@export
*/
[[cpp4r::register]] double get_by_name_(doubles x, std::string name) {
  return x[name.c_str()];
}

/* roxygen
@title Check if Named Vector Contains Name on 'C++' Side
@description Test suite
@param x named vector to query
@param name name of the element to check
@export
*/
[[cpp4r::register]] bool contains_name_(doubles x, std::string name) {
  return x.contains(name.c_str());
}

/* roxygen
@title Find Position of Name in Named Vector on 'C++' Side
@description Test suite
@param x named vector to query
@param name name of the element to find
@export
*/
[[cpp4r::register]] int find_name_pos_(doubles x, std::string name) {
  auto it = x.find(name.c_str());
  if (it == x.end()) {
    return -1;
  }
  return static_cast<int>(std::distance(x.begin(), it)) + 1;  // 1-indexed
}
