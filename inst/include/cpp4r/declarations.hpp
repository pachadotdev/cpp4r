#pragma once

#ifndef CPP4R_PARTIAL
#include "cpp4r.hpp"  // Includes cpp_version.hpp
#else
#include "cpp4r/cpp_version.hpp"  // Need version detection even in partial mode
#endif

#include <cstring>
#include <string>
#include <vector>

// Davis: From what I can tell, you'd only ever define this if you need to include
// `declarations.hpp` manually in a file, i.e. to possibly use `BEGIN_CPP4R` with a
// custom `END_CPP4R`, as textshaping does do. Otherwise, `declarations.hpp` is included
// in `code.cpp` and should contain all of the cpp4r type definitions that the generated
// function signatures need to link against.
#ifndef CPP4R_PARTIAL
namespace writable = ::cpp4r::writable;
using namespace ::cpp4r;
#endif

#include <R_ext/Rdynload.h>

#define CPP4R_ERROR_BUFSIZE 8192

#if CPP4R_HAS_CXX20
// C++20: Use [[likely]]/[[unlikely]] for better branch prediction
#define BEGIN_CPP4R                   \
  SEXP err = R_NilValue;              \
  char buf[CPP4R_ERROR_BUFSIZE] = ""; \
  try {
#define END_CPP4R                                               \
  }                                                             \
  catch (cpp4r::unwind_exception & e) {                         \
    err = e.token;                                              \
  }                                                             \
  catch (std::exception & e) {                                  \
    strncpy(buf, e.what(), sizeof(buf) - 1);                    \
  }                                                             \
  catch (...) {                                                 \
    strncpy(buf, "C++ error (unknown cause)", sizeof(buf) - 1); \
  }                                                             \
  if (buf[0] != '\0') CPP4R_UNLIKELY {                          \
    Rf_errorcall(R_NilValue, "%s", buf);                        \
  } else if (err != R_NilValue) CPP4R_UNLIKELY {                \
    R_ContinueUnwind(err);                                      \
  }                                                             \
  return R_NilValue;

#else
// C++11/14/17: Standard macro without branch hints
#define BEGIN_CPP4R                   \
  SEXP err = R_NilValue;              \
  char buf[CPP4R_ERROR_BUFSIZE] = ""; \
  try {
#define END_CPP4R                                               \
  }                                                             \
  catch (cpp4r::unwind_exception & e) {                         \
    err = e.token;                                              \
  }                                                             \
  catch (std::exception & e) {                                  \
    strncpy(buf, e.what(), sizeof(buf) - 1);                    \
  }                                                             \
  catch (...) {                                                 \
    strncpy(buf, "C++ error (unknown cause)", sizeof(buf) - 1); \
  }                                                             \
  if (buf[0] != '\0') {                                         \
    Rf_errorcall(R_NilValue, "%s", buf);                        \
  } else if (err != R_NilValue) {                               \
    R_ContinueUnwind(err);                                      \
  }                                                             \
  return R_NilValue;
#endif
