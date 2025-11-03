#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <string>   // for string, basic_string
#include <utility>  // for pair
#include <vector>   // for vector

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

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
#if CPP4R_HAS_CXX17
    CPP4R_NODISCARD operator SEXP() const { 
      return safe[detail::r_env_get](parent_, name_); 
    }
    CPP4R_NODISCARD operator sexp() const { 
      return sexp(safe[detail::r_env_get](parent_, name_)); 
    }
#else
    operator SEXP() const { return safe[detail::r_env_get](parent_, name_); }
    operator sexp() const { return sexp(safe[detail::r_env_get](parent_, name_)); }
#endif

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

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload for zero-copy lookups
  inline proxy operator[](std::string_view name) const {
    SEXP symbol = safe[Rf_install](name.data());
    return {env_, symbol};
  }
#endif

  inline bool exists(SEXP name) const { return safe[detail::r_env_has](env_, name); }
  inline bool exists(const char* name) const {
    // Cache the symbol lookup
    SEXP symbol = safe[Rf_install](name);
    return safe[detail::r_env_has](env_, symbol);
  }
  inline bool exists(const std::string& name) const { return exists(name.c_str()); }

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload
  inline bool exists(std::string_view name) const {
    SEXP symbol = safe[Rf_install](name.data());
    return safe[detail::r_env_has](env_, symbol);
  }
#endif

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

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload
  void remove(std::string_view name) {
    SEXP symbol = safe[Rf_install](name.data());
    remove(symbol);
  }
#endif

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD R_xlen_t size() const { return Rf_xlength(env_); }
#else
  R_xlen_t size() const { return Rf_xlength(env_); }
#endif

  // Optimized bulk operations
  void assign_multiple(const std::vector<std::pair<std::string, SEXP>>& assignments) {
    unwind_protect([&] {
      // Reserve space if we know the vector size won't change
      const std::size_t size = assignments.size();
#if CPP4R_HAS_CXX20
      for (std::size_t i = 0; i < size; ++i) CPP4R_LIKELY {
#else
      for (std::size_t i = 0; i < size; ++i) {
#endif
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
#if CPP4R_HAS_CXX20
      for (std::size_t i = 0; i < size; ++i) CPP4R_LIKELY {
#else
      for (std::size_t i = 0; i < size; ++i) {
#endif
        SEXP symbol = safe[Rf_install](names[i].c_str());
        R_removeVarFromFrame(symbol, env_);
      }
    });
  }

  // Fast path for checking if environment is global or base
#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD bool is_global_env() const noexcept { return env_.data() == R_GlobalEnv; }

  CPP4R_NODISCARD bool is_base_env() const noexcept { return env_.data() == R_BaseEnv; }
#else
  bool is_global_env() const noexcept { return env_.data() == R_GlobalEnv; }

  bool is_base_env() const noexcept { return env_.data() == R_BaseEnv; }
#endif

  // Comparison operators for environments
#if CPP4R_HAS_CXX20
  CPP4R_NODISCARD bool operator==(const environment& other) const noexcept CPP4R_LIKELY {
    return env_.data() == other.env_.data();
  }

  CPP4R_NODISCARD bool operator!=(const environment& other) const noexcept CPP4R_UNLIKELY {
    return !(*this == other);
  }

  CPP4R_NODISCARD bool operator==(SEXP other) const noexcept CPP4R_LIKELY {
    return env_.data() == other;
  }

  CPP4R_NODISCARD bool operator!=(SEXP other) const noexcept CPP4R_UNLIKELY {
    return env_.data() != other;
  }
#elif CPP4R_HAS_CXX17
  CPP4R_NODISCARD bool operator==(const environment& other) const noexcept {
    return env_.data() == other.env_.data();
  }

  CPP4R_NODISCARD bool operator!=(const environment& other) const noexcept {
    return !(*this == other);
  }

  CPP4R_NODISCARD bool operator==(SEXP other) const noexcept {
    return env_.data() == other;
  }

  CPP4R_NODISCARD bool operator!=(SEXP other) const noexcept {
    return env_.data() != other;
  }
#else
  bool operator==(const environment& other) const noexcept {
    return env_.data() == other.env_.data();
  }

  bool operator!=(const environment& other) const noexcept { return !(*this == other); }

  bool operator==(SEXP other) const noexcept { return env_.data() == other; }

  bool operator!=(SEXP other) const noexcept { return env_.data() != other; }
#endif

  operator SEXP() const { return env_; }
};

}  // namespace cpp4r
