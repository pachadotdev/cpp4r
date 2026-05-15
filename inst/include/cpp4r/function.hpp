#pragma once

#include <cstring>  // for std::strcmp (@pachadotdev use std qualifiers)

#include <cstdio>   // for snprintf
#include <string>   // for string, basic_string
#include <utility>  // for forward

#include "cpp4r/R.hpp"          // for R’s C interface (e.g., for SEXP)
#include "cpp4r/as.hpp"         // for as_sexp
#include "cpp4r/named_arg.hpp"  // for named_arg
#include "cpp4r/protect.hpp"    // for safe, protect, etc.
#include "cpp4r/sexp.hpp"       // for sexp

namespace cpp4r {

// Result of a try_call() — holds the value on success, or an error flag and
// the R error message on failure.
struct call_result {
  sexp value;
  bool error;

  // Returns the R error message when error == true, nullptr otherwise.
  // The buffer is owned by R and valid until the next R error.
  const char* error_message() const noexcept { return error ? R_curErrorBuf() : nullptr; }

  explicit operator bool() const noexcept { return !error; }
};

class function {
 public:
  // Default constructor: data_ is R_NilValue (via sexp's default constructor).
  // Needed for storing a cpp4r::function as an uninitialized class member.
  function() noexcept = default;

  function(SEXP data) : data_(data) {}

  // Construct from a sexp, e.g. when a function was received as a generic sexp.
  function(const sexp& data) : data_(data) {}
  function(sexp&& data) : data_(std::move(data)) {}

  // Copy/move: delegate to sexp's, which handle protect/unprotect correctly.
  function(const function&) = default;
  function(function&&) = default;
  function& operator=(const function&) = default;
  function& operator=(function&&) = default;

  operator SEXP() const noexcept { return data_; }
  // Return the underlying sexp for interop with sexp-based APIs.
  sexp data() const noexcept { return data_; }

  // Evaluate the function in R_GlobalEnv, throwing on error (via unwind_protect).
  template <typename... Args>
  sexp operator()(Args&&... args) const {
    return eval_in(R_GlobalEnv, std::forward<Args>(args)...);
  }

  // Evaluate the function in a specific environment, throwing on error.
  template <typename... Args>
  sexp call_in(SEXP env, Args&&... args) const {
    return eval_in(env, std::forward<Args>(args)...);
  }

  // Evaluate the function silently via R_tryEvalSilent. Never throws.
  // On error, result.error == true and result.error_message() returns the
  // R error text; result.value == R_NilValue.
  template <typename... Args>
  call_result try_call(Args&&... args) const {
    return try_eval_in(R_GlobalEnv, std::forward<Args>(args)...);
  }

  // Same as try_call() but evaluates in a specific environment.
  template <typename... Args>
  call_result try_call_in(SEXP env, Args&&... args) const {
    return try_eval_in(env, std::forward<Args>(args)...);
  }

 private:
  sexp data_;

  template <typename... Args>
  sexp make_call(Args&&... args) const {
    R_xlen_t num_args = sizeof...(args) + 1;
    sexp call(safe[Rf_allocVector](LANGSXP, num_args));
    construct_call(call, data_, std::forward<Args>(args)...);
    return call;
  }

  template <typename... Args>
  sexp eval_in(SEXP env, Args&&... args) const {
    sexp call = make_call(std::forward<Args>(args)...);
    return safe[Rf_eval](call, env);
  }

  template <typename... Args>
  call_result try_eval_in(SEXP env, Args&&... args) const {
    sexp call = make_call(std::forward<Args>(args)...);
    int error_flag = 0;
    SEXP result = R_tryEvalSilent(call, env, &error_flag);
    return {sexp(error_flag ? R_NilValue : result), error_flag != 0};
  }

  template <typename... Args>
  void construct_call(SEXP val, const named_arg& arg, Args&&... args) const {
    SETCAR(val, arg.value());
    SET_TAG(val, safe[Rf_install](arg.name()));
    val = CDR(val);
    construct_call(val, std::forward<Args>(args)...);
  }

  // Construct the call recursively, each iteration adds an Arg to the pairlist.
  template <typename T, typename... Args>
  void construct_call(SEXP val, const T& arg, Args&&... args) const {
    SETCAR(val, cpp4r::as_sexp(arg));
    val = CDR(val);
    construct_call(val, std::forward<Args>(args)...);
  }

  // Base case, just return
  void construct_call(SEXP val) const {}
};

class package {
 public:
  package(const char* name) : data_(get_namespace(name)) {}
  package(const std::string& name) : data_(get_namespace(name.c_str())) {}
  function operator[](const char* name) {
    return safe[Rf_findFun](safe[Rf_install](name), data_);
  }
  function operator[](const std::string& name) { return operator[](name.c_str()); }

 private:
  static SEXP get_namespace(const char* name) {
    if (strcmp(name, "base") == 0) {
      return R_BaseEnv;
    }
    sexp name_sexp = safe[Rf_mkString](name);
    return safe[R_FindNamespace](name_sexp);
  }

  // Either base env or in namespace registry, so no protection needed
  SEXP data_;
};

namespace detail {

// Special internal way to call `base::message()`
//
// - Pure C, so call with `safe[]`
// - Holds a `static SEXP` for the `base::message` function protected with
// `R_PreserveObject()`
//
// We don't use a `static cpp4r::function` because that will infinitely retain a cell in
// our preserve list, which can throw off our counts in the preserve list tests.
inline void r_message(const char* x) {
  static SEXP fn = NULL;

  if (fn == NULL) {
    fn = Rf_findFun(Rf_install("message"), R_BaseEnv);
    R_PreserveObject(fn);
  }

  SEXP x_char = PROTECT(Rf_mkCharCE(x, CE_UTF8));
  SEXP x_string = PROTECT(Rf_ScalarString(x_char));

  SEXP call = PROTECT(Rf_lang2(fn, x_string));

  Rf_eval(call, R_GlobalEnv);

  UNPROTECT(3);
}

}  // namespace detail

inline void message(const char* fmt_arg) {
  char buff[1024];
  int msg = std::snprintf(buff, 1024, "%s", fmt_arg);
  if (msg >= 0 && msg < 1024) {
    safe[detail::r_message](buff);
  }
}

template <typename... Args>
void message(const char* fmt_arg, Args... args) {
  char buff[1024];
  int msg = std::snprintf(buff, 1024, fmt_arg, args...);
  if (msg >= 0 && msg < 1024) {
    safe[detail::r_message](buff);
  }
}

inline void message(const std::string& fmt_arg) { message(fmt_arg.c_str()); }

template <typename... Args>
void message(const std::string& fmt_arg, Args... args) {
  message(fmt_arg.c_str(), args...);
}

}  // namespace cpp4r
