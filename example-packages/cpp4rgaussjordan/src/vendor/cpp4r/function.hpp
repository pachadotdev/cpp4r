#pragma once

#include <cstring>  // for std::strcmp (@pachadotdev use std qualifiers)

#include <cstdio>   // for snprintf
#include <string>   // for string, basic_string
#include <utility>  // for forward

#include "cpp4r/R.hpp"          // for SEXP, SEXPREC, CDR, Rf_install, SETCAR
#include "cpp4r/as.hpp"         // for as_sexp
#include "cpp4r/named_arg.hpp"  // for named_arg
#include "cpp4r/protect.hpp"    // for protect, protect::function, safe
#include "cpp4r/sexp.hpp"       // for sexp

namespace cpp4r {

class function {
 public:
  inline function(SEXP data) noexcept : data_(data) {}

  template <typename... Args>
  inline sexp operator()(Args&&... args) const {
    // Size of the arguments plus one for the function name itself
    constexpr R_xlen_t num_args = sizeof...(args) + 1;

    sexp call(safe[Rf_allocVector](LANGSXP, num_args));

    construct_call(call, data_, std::forward<Args>(args)...);

    return safe[Rf_eval](call, R_GlobalEnv);
  }

 private:
  sexp data_;

  template <typename... Args>
  inline void construct_call(SEXP val, const named_arg& arg, Args&&... args) const {
    SETCAR(val, arg.value());
    SET_TAG(val, safe[Rf_install](arg.name()));
    val = CDR(val);
    construct_call(val, std::forward<Args>(args)...);
  }

  // Construct the call recursively, each iteration adds an Arg to the pairlist.
  template <typename T, typename... Args>
  inline void construct_call(SEXP val, const T& arg, Args&&... args) const {
    SETCAR(val, as_sexp(arg));
    val = CDR(val);
    construct_call(val, std::forward<Args>(args)...);
  }

  // Base case, just return
  inline void construct_call(SEXP val) const noexcept {}
};

class package {
 public:
  inline package(const char* name) : data_(get_namespace(name)) {}
  inline package(const std::string& name) : data_(get_namespace(name.c_str())) {}
  inline function operator[](const char* name) {
    return safe[Rf_findFun](safe[Rf_install](name), data_);
  }
  inline function operator[](const std::string& name) { return operator[](name.c_str()); }

 private:
  static inline SEXP get_namespace(const char* name) {
    if (__builtin_expect(std::strcmp(name, "base") == 0, 1)) {
      return R_BaseEnv;
    }
    sexp name_sexp = safe[Rf_install](name);
    return safe[detail::r_env_get](R_NamespaceRegistry, name_sexp);
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
#ifdef CPP4R_USE_FMT
  std::string msg = fmt::format(fmt_arg);
  safe[detail::r_message](msg.c_str());
#else
  char buff[1024];
  int msg;
  msg = std::snprintf(buff, 1024, "%s", fmt_arg);
  if (msg >= 0 && msg < 1024) {
    safe[detail::r_message](buff);
  }
#endif
}

template <typename... Args>
void message(const char* fmt_arg, Args... args) {
#ifdef CPP4R_USE_FMT
  std::string msg = fmt::format(fmt_arg, args...);
  safe[detail::r_message](msg.c_str());
#else
  char buff[1024];
  int msg;
  msg = std::snprintf(buff, 1024, fmt_arg, args...);
  if (msg >= 0 && msg < 1024) {
    safe[detail::r_message](buff);
  }
#endif
}

inline void message(const std::string& fmt_arg) { message(fmt_arg.c_str()); }

template <typename... Args>
void message(const std::string& fmt_arg, Args... args) {
  message(fmt_arg.c_str(), args...);
}

}  // namespace cpp4r
