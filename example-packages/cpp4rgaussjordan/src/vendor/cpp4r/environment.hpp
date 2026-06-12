#pragma once

#include <string>   // for string, basic_string
#include <utility>  // for pair
#include <vector>   // for vector

#include "cpp4r/R.hpp"        // for SEXP, SEXPREC, Rf_install, r_env_get...
#include "cpp4r/as.hpp"       // for as_sexp
#include "cpp4r/protect.hpp"  // for protect, protect::function, safe, unwin...
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

    // Copy and move constructors for better performance
    proxy(const proxy&) = default;
    proxy(proxy&&) noexcept = default;

    template <typename T>
    proxy& operator=(T&& value) {
      safe[Rf_defineVar](name_, as_sexp(std::forward<T>(value)), parent_);
      return *this;
    }

    // Optimized conversion operators - cache the result for reuse
    operator SEXP() const { return safe[detail::r_env_get](parent_, name_); }
    operator sexp() const { return sexp(safe[detail::r_env_get](parent_, name_)); }

    // Add comparison operators
    template <typename T>
    bool operator==(const T& other) const {
      return static_cast<SEXP>(*this) == as_sexp(other);
    }

    template <typename T>
    bool operator!=(const T& other) const {
      return !(*this == other);
    }
  };

 public:
  environment(SEXP env) : env_(env) {}
  environment(sexp env) : env_(env) {}

  // Copy constructor and assignment for efficient copying
  environment(const environment& other) = default;
  environment& operator=(const environment& other) = default;

  // Move constructor and assignment for efficient moving
  environment(environment&& other) noexcept = default;
  environment& operator=(environment&& other) noexcept = default;
  inline proxy operator[](const SEXP name) const noexcept { return {env_, name}; }
  inline proxy operator[](const char* name) const {
    // Cache the symbol lookup
    SEXP symbol = safe[Rf_install](name);
    return {env_, symbol};
  }
  inline proxy operator[](const std::string& name) const {
    return operator[](name.c_str());
  }

  inline bool exists(SEXP name) const { return safe[detail::r_env_has](env_, name); }
  inline bool exists(const char* name) const {
    // Cache the symbol lookup
    SEXP symbol = safe[Rf_install](name);
    return safe[detail::r_env_has](env_, symbol);
  }
  inline bool exists(const std::string& name) const { return exists(name.c_str()); }

  void remove(SEXP name) {
    // Use unwind_protect instead of manual PROTECT/UNPROTECT
    unwind_protect([&] { R_removeVarFromFrame(name, env_); });
  }

  void remove(const char* name) {
    // Cache the symbol lookup and call remove directly
    SEXP symbol = safe[Rf_install](name);
    remove(symbol);
  }

  void remove(const std::string& name) { return remove(name.c_str()); }

  R_xlen_t size() const { return Rf_xlength(env_); }

  // Optimized bulk operations
  void assign_multiple(const std::vector<std::pair<std::string, SEXP>>& assignments) {
    unwind_protect([&] {
      // Reserve space if we know the vector size won't change
      const std::size_t size = assignments.size();
      for (std::size_t i = 0; i < size; ++i) {
        const auto& pair = assignments[i];
        SEXP symbol = safe[Rf_install](pair.first.c_str());
        safe[Rf_defineVar](symbol, pair.second, env_);
      }
    });
  }

  void remove_multiple(const std::vector<std::string>& names) {
    unwind_protect([&] {
      // Use indexed loop for better performance
      const std::size_t size = names.size();
      for (std::size_t i = 0; i < size; ++i) {
        SEXP symbol = safe[Rf_install](names[i].c_str());
        R_removeVarFromFrame(symbol, env_);
      }
    });
  }

  // Fast path for checking if environment is global or base
  bool is_global_env() const noexcept { return env_.data() == R_GlobalEnv; }

  bool is_base_env() const noexcept { return env_.data() == R_BaseEnv; }

  // Comparison operators for environments
  bool operator==(const environment& other) const noexcept {
    return env_.data() == other.env_.data();
  }

  bool operator!=(const environment& other) const noexcept { return !(*this == other); }

  bool operator==(SEXP other) const noexcept { return env_.data() == other; }

  bool operator!=(SEXP other) const noexcept { return env_.data() != other; }

  operator SEXP() const { return env_; }
};

}  // namespace cpp4r
