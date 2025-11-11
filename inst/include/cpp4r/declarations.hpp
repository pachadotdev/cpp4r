#pragma once

#include <cstring>
#include <string>
#include <vector>

// Davis: From what I can tell, you'd only ever define this if you need to include
// `declarations.hpp` manually in a file, i.e. to possibly use `BEGIN_cpp4r` with a
// custom `END_cpp4r`, as textshaping does do. Otherwise, `declarations.hpp` is included
// in `code.cpp` and should contain all of the cpp4r type definitions that the generated
// function signatures need to link against.
#ifndef cpp4r_PARTIAL
#include "cpp4r.hpp"
namespace writable = ::cpp4r::writable;
using namespace ::cpp4r;
#endif

#include <R_ext/Rdynload.h>

#define cpp4r_ERROR_BUFSIZE 8192

#define BEGIN_cpp4r                   \
  SEXP err = R_NilValue;              \
  char buf[cpp4r_ERROR_BUFSIZE] = ""; \
  try {
#define END_cpp4r                                               \
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
