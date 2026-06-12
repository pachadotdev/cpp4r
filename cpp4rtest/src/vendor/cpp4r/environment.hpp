#pragma once

#include <string>  // for string, basic_string

// C++17+: std::string_view for zero-cost string argument passing
#if CPP4R_HAS_CXX17
#include <string_view>
#endif

#include "cpp4r/R.hpp"        // for R’s C interface (e.g., for SEXP)
#include "cpp4r/as.hpp"       // for as_sexp
#include "cpp4r/protect.hpp"  // for safe, protect, etc.
#include "cpp4r/sexp.hpp"     // for sexp

namespace cpp4r {

class environment {
 private:
  sexp env_;

  class proxy {
    SEXP parent_;
    SEXP name_;

   public:
    proxy(SEXP parent, SEXP name) : parent_(parent), name_(name) {}

    template <typename T>
    proxy& operator=(T value) {
      safe[Rf_defineVar](name_, as_sexp(value), parent_);
      return *this;
    }
    operator SEXP() const { return safe[detail::r_env_get](parent_, name_); };
    operator sexp() const { return SEXP(); };
  };

 public:
  environment(SEXP env) : env_(env) {}
  environment(sexp env) : env_(env) {}

  // Create a new environment (R >= 4.1.0).
  // enclos: enclosing environment (parent).
  // hash:   non-zero to enable hashing; recommended for environments with
  //         many bindings.
  // size:   initial hash table size hint (ignored when hash == 0).
  static environment new_env(SEXP enclos = R_GlobalEnv, int hash = 1, int size = 29) {
    return environment(safe[R_NewEnv](enclos, hash, size));
  }

  // Well-known environment singletons.
  static environment global_env() noexcept { return environment(R_GlobalEnv); }
  static environment base_env() noexcept { return environment(R_BaseEnv); }
  static environment empty_env() noexcept { return environment(R_EmptyEnv); }

  proxy operator[](const SEXP name) const { return {env_, name}; }
  proxy operator[](const char* name) const { return operator[](safe[Rf_install](name)); }
  proxy operator[](const std::string& name) const { return operator[](name.c_str()); }
#if CPP4R_HAS_CXX17
  // C++17+: accept string_view directly, avoiding a caller-side std::string construction
  proxy operator[](std::string_view name) const {
    std::string s(name);
    return operator[](s.c_str());
  }
#endif

  bool exists(SEXP name) const { return safe[detail::r_env_has](env_, name); }
  bool exists(const char* name) const { return exists(safe[Rf_install](name)); }
  bool exists(const std::string& name) const { return exists(name.c_str()); }
#if CPP4R_HAS_CXX17
  bool exists(std::string_view name) const {
    std::string s(name);
    return exists(s.c_str());
  }
#endif

  void remove(SEXP name) {
    PROTECT(name);
    R_removeVarFromFrame(name, env_);
    UNPROTECT(1);
  }

  void remove(const char* name) { remove(safe[Rf_install](name)); }

  // Lock this environment, preventing new bindings from being added.
  // If bindings is true, all existing bindings are also locked (read-only).
  void lock(bool bindings = false) noexcept {
    R_LockEnvironment(env_, bindings ? TRUE : FALSE);
  }

  bool is_locked() const noexcept { return R_EnvironmentIsLocked(env_) == TRUE; }

  // List names of bindings in this environment (does not search enclosing
  // envs). Returns a sorted STRSXP; wrap in cpp4r::strings() for iteration.
  // Pass all_names = true to include names beginning with '.'.
  sexp ls(bool all_names = false) const {
    return safe[R_lsInternal3](env_, all_names ? TRUE : FALSE, TRUE);
  }

  R_xlen_t size() const noexcept { return Rf_xlength(env_); }

  operator SEXP() const noexcept { return env_; }
};

}  // namespace cpp4r
