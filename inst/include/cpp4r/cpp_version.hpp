#pragma once

// C++ Version Detection for cpp4r
// Provides feature detection macros for various C++ standards

// Detect C++14
#if __cplusplus >= 201402L
  #define CPP4R_HAS_CXX14 1
#else
  #define CPP4R_HAS_CXX14 0
#endif

// Detect C++17
#if __cplusplus >= 201703L
  #define CPP4R_HAS_CXX17 1
#else
  #define CPP4R_HAS_CXX17 0
#endif

// Detect C++20
#if __cplusplus >= 202002L
  #define CPP4R_HAS_CXX20 1
#else
  #define CPP4R_HAS_CXX20 0
#endif

// Detect C++23
#if __cplusplus >= 202302L
  #define CPP4R_HAS_CXX23 1
#else
  #define CPP4R_HAS_CXX23 0
#endif

// Feature-specific detection

// C++14 features
#if CPP4R_HAS_CXX14
  #define CPP4R_HAS_VARIABLE_TEMPLATES 1
  #define CPP4R_HAS_GENERIC_LAMBDAS 1
  #define CPP4R_HAS_RELAXED_CONSTEXPR 1
#else
  #define CPP4R_HAS_VARIABLE_TEMPLATES 0
  #define CPP4R_HAS_GENERIC_LAMBDAS 0
  #define CPP4R_HAS_RELAXED_CONSTEXPR 0
#endif

// C++17 features
#if CPP4R_HAS_CXX17
  #define CPP4R_HAS_IF_CONSTEXPR 1
  #define CPP4R_HAS_INLINE_VARIABLES 1
  #define CPP4R_HAS_STRUCTURED_BINDINGS 1
  #define CPP4R_HAS_FOLD_EXPRESSIONS 1
  #define CPP4R_HAS_STD_OPTIONAL 1
#else
  #define CPP4R_HAS_IF_CONSTEXPR 0
  #define CPP4R_HAS_INLINE_VARIABLES 0
  #define CPP4R_HAS_STRUCTURED_BINDINGS 0
  #define CPP4R_HAS_FOLD_EXPRESSIONS 0
  #define CPP4R_HAS_STD_OPTIONAL 0
#endif

// C++20 features
#if CPP4R_HAS_CXX20
  #define CPP4R_HAS_CONCEPTS 1
  #define CPP4R_HAS_RANGES 1
  #define CPP4R_HAS_CONSTEVAL 1
  #define CPP4R_HAS_STD_SPAN 1
#else
  #define CPP4R_HAS_CONCEPTS 0
  #define CPP4R_HAS_RANGES 0
  #define CPP4R_HAS_CONSTEVAL 0
  #define CPP4R_HAS_STD_SPAN 0
#endif

// C++23 features
#if CPP4R_HAS_CXX23
  #define CPP4R_HAS_STD_EXPECTED 1
  #define CPP4R_HAS_MULTIDIMENSIONAL_SUBSCRIPT 1
  #define CPP4R_HAS_IF_CONSTEVAL 1
#else
  #define CPP4R_HAS_STD_EXPECTED 0
  #define CPP4R_HAS_MULTIDIMENSIONAL_SUBSCRIPT 0
  #define CPP4R_HAS_IF_CONSTEVAL 0
#endif

// Compiler-specific optimizations
#ifdef __GNUC__
  #define CPP4R_HAS_BUILTIN_EXPECT 1
#else
  #define CPP4R_HAS_BUILTIN_EXPECT 0
#endif

// Utility macros for conditional compilation
#if CPP4R_HAS_IF_CONSTEXPR
  #define CPP4R_IF_CONSTEXPR if constexpr
#else
  #define CPP4R_IF_CONSTEXPR if
#endif

#if CPP4R_HAS_CXX17
  #define CPP4R_NODISCARD [[nodiscard]]
#else
  #define CPP4R_NODISCARD
#endif

// CPP4R_LIKELY and CPP4R_UNLIKELY: branch prediction hints
// These remain as function-like macros for consistency across all C++ versions
// In C++20+, the compiler is smart enough to optimize these patterns anyway
#if defined(__GNUC__) || defined(__clang__)
  #define CPP4R_LIKELY(x)   __builtin_expect(!!(x), 1)
  #define CPP4R_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
  #define CPP4R_LIKELY(x)   (x)
  #define CPP4R_UNLIKELY(x) (x)
#endif
