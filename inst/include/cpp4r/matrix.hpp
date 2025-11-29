#pragma once

#include <initializer_list>  // for initializer_list
#include <iterator>
#include <string>  // for string

#include "cpp4r/R.hpp"                // for SEXP, SEXPREC, R_xlen_t, INT...
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/cpp_version.hpp"      // for CPP4R feature detection
#include "cpp4r/r_bool.hpp"           // for r_bool
#include "cpp4r/r_string.hpp"         // for r_string
#include "cpp4r/r_vector.hpp"         // for r_vector
#include "cpp4r/sexp.hpp"             // for sexp

namespace cpp4r {

namespace detail {
template <typename T>
struct get_sexptype_v;
template <>
struct get_sexptype_v<double> {
  static constexpr SEXPTYPE value = REALSXP;
};
template <>
struct get_sexptype_v<int> {
  static constexpr SEXPTYPE value = INTSXP;
};
template <>
struct get_sexptype_v<r_bool> {
  static constexpr SEXPTYPE value = LGLSXP;
};
template <>
struct get_sexptype_v<r_complex> {
  static constexpr SEXPTYPE value = CPLXSXP;
};
template <>
struct get_sexptype_v<r_string> {
  static constexpr SEXPTYPE value = STRSXP;
};
}  // namespace detail

// matrix dimensions
struct matrix_dims {
 protected:
  const int nrow_;
  const int ncol_;

 public:
  matrix_dims(SEXP data) : nrow_(Rf_nrows(data)), ncol_(Rf_ncols(data)) {}
  matrix_dims(int nrow, int ncol) : nrow_(nrow), ncol_(ncol) {}

  CPP4R_ALWAYS_INLINE int nrow() const noexcept { return nrow_; }
  CPP4R_ALWAYS_INLINE int ncol() const noexcept { return ncol_; }
};

// base type for dimension-wise matrix access specialization
struct matrix_slice {};

struct by_row : public matrix_slice {};
struct by_column : public matrix_slice {};

// basic properties of matrix slices
template <typename S>
struct matrix_slices : public matrix_dims {
 public:
  using matrix_dims::matrix_dims;
  using matrix_dims::ncol;
  using matrix_dims::nrow;

  int nslices() const;
  int slice_size() const;
  int slice_stride() const;
  int slice_offset(int pos) const;
};

// basic properties of matrix row slices
template <>
struct matrix_slices<by_row> : public matrix_dims {
 public:
  using matrix_dims::matrix_dims;
  using matrix_dims::ncol;
  using matrix_dims::nrow;

  CPP4R_ALWAYS_INLINE int nslices() const noexcept { return nrow(); }
  CPP4R_ALWAYS_INLINE int slice_size() const noexcept { return ncol(); }
  CPP4R_ALWAYS_INLINE int slice_stride() const noexcept { return nrow(); }
  CPP4R_ALWAYS_INLINE int slice_offset(int pos) const noexcept { return pos; }
};

// basic properties of matrix column slices
template <>
struct matrix_slices<by_column> : public matrix_dims {
 public:
  using matrix_dims::matrix_dims;
  using matrix_dims::ncol;
  using matrix_dims::nrow;

  CPP4R_ALWAYS_INLINE int nslices() const noexcept { return ncol(); }
  CPP4R_ALWAYS_INLINE int slice_size() const noexcept { return nrow(); }
  CPP4R_ALWAYS_INLINE int slice_stride() const noexcept { return 1; }
  CPP4R_ALWAYS_INLINE int slice_offset(int pos) const noexcept { return pos * nrow(); }
};

template <typename V, typename T, typename S = by_column>
class matrix : public matrix_slices<S> {
 private:
  V vector_;

  template <typename V2, typename T2, typename S2>
  friend class matrix;

 public:
  using underlying_type = typename V::underlying_type;

  // matrix slice: row (if S=by_row) or a column (if S=by_column)
  class slice {
   private:
    const matrix& parent_;
    int index_;   // slice index
    int offset_;  // index of the first slice element in parent_.vector_

   public:
    slice(const matrix& parent, int index)
        : parent_(parent), index_(index), offset_(parent.slice_offset(index)) {}

    R_xlen_t stride() const noexcept { return parent_.slice_stride(); }
    R_xlen_t size() const noexcept { return parent_.slice_size(); }

    bool operator==(const slice& rhs) const noexcept {
      return (index_ == rhs.index_) && (parent_.data() == rhs.parent_.data());
    }
    bool operator!=(const slice& rhs) const noexcept { return !operator==(rhs); }

    CPP4R_ALWAYS_INLINE T operator[](int pos) const {
      return parent_.vector_[offset_ + stride() * pos];
    }

    // iterates elements of a slice
    class iterator {
     private:
      const slice& slice_;
      int pos_;

     public:
      using difference_type = std::ptrdiff_t;
      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator_category = std::forward_iterator_tag;

      iterator(const slice& slice, R_xlen_t pos) : slice_(slice), pos_(pos) {}

      iterator& operator++() {
        ++pos_;
        return *this;
      }

      bool operator==(const iterator& rhs) const {
        return (pos_ == rhs.pos_) && (slice_ == rhs.slice_);
      }
      bool operator!=(const iterator& rhs) const { return !operator==(rhs); }

      T operator*() const { return slice_[pos_]; };
    };

    iterator begin() const { return {*this, 0}; }
    iterator end() const { return {*this, size()}; }
  };
  friend slice;

  // iterates slices (rows or columns -- depending on S template param) of a matrix
  class slice_iterator {
   private:
    const matrix& parent_;
    int pos_;

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = slice;
    using pointer = slice*;
    using reference = slice&;
    using iterator_category = std::forward_iterator_tag;

    slice_iterator(const matrix& parent, R_xlen_t pos) : parent_(parent), pos_(pos) {}

    slice_iterator& operator++() {
      ++pos_;
      return *this;
    }

    bool operator==(const slice_iterator& rhs) const {
      return (pos_ == rhs.pos_) && (parent_.data() == rhs.parent_.data());
    }
    bool operator!=(const slice_iterator& rhs) const { return !operator==(rhs); }

    slice operator*() { return parent_[pos_]; };
  };

 public:
  matrix(SEXP data) : matrix_slices<S>(data), vector_(data) {}

  template <typename V2, typename T2, typename S2>
  matrix(const cpp4r::matrix<V2, T2, S2>& rhs)
      : matrix_slices<S>(rhs.nrow(), rhs.ncol()), vector_(rhs.vector_) {}

  template <typename V2, typename T2, typename S2>
  matrix(cpp4r::matrix<V2, T2, S2>&& rhs)
      : matrix_slices<S>(rhs.nrow(), rhs.ncol()), vector_(std::move(rhs.vector_)) {}

  // Optimized constructor for writable matrices to minimize allocation overhead
  // Uses fast-path r_vector constructor that bypasses type validation and ALTREP checks
  CPP4R_ALWAYS_INLINE matrix(int nrow, int ncol)
      : matrix_slices<S>(nrow, ncol),
        vector_(Rf_allocMatrix(detail::get_sexptype_v<typename V::scalar_type>::value,
                               nrow, ncol),
                writable::fresh_allocation_tag{}) {}

  using matrix_slices<S>::nrow;
  using matrix_slices<S>::ncol;
  using matrix_slices<S>::nslices;
  using matrix_slices<S>::slice_size;
  using matrix_slices<S>::slice_stride;
  using matrix_slices<S>::slice_offset;

  V vector() const { return vector_; }

  SEXP data() const { return vector_.data(); }

  R_xlen_t size() const { return vector_.size(); }

  operator SEXP() const { return SEXP(vector_); }

  // operator sexp() { return sexp(vector_); }

  attribute_proxy<V> attr(const char* name) { return attribute_proxy<V>(vector_, name); }

  attribute_proxy<V> attr(const std::string& name) {
    return attribute_proxy<V>(vector_, name.c_str());
  }

  attribute_proxy<V> attr(SEXP name) { return attribute_proxy<V>(vector_, name); }

  void attr(const char* name, SEXP value) { vector_.attr(name) = value; }

  void attr(const std::string& name, SEXP value) { vector_.attr(name) = value; }

  void attr(SEXP name, SEXP value) { vector_.attr(name) = value; }

  void attr(const char* name, std::initializer_list<SEXP> value) {
    SEXP attr = PROTECT(Rf_allocVector(VECSXP, value.size()));
    int i = 0;
    for (SEXP v : value) {
      SET_VECTOR_ELT(attr, i++, v);
    }
    vector_.attr(name) = attr;
    UNPROTECT(1);
  }

  void attr(const std::string& name, std::initializer_list<SEXP> value) {
    SEXP attr = PROTECT(Rf_allocVector(VECSXP, value.size()));
    int i = 0;
    for (SEXP v : value) {
      SET_VECTOR_ELT(attr, i++, v);
    }
    vector_.attr(name) = attr;
    UNPROTECT(1);
  }

  void attr(SEXP name, std::initializer_list<SEXP> value) {
    SEXP attr = PROTECT(Rf_allocVector(VECSXP, value.size()));
    int i = 0;
    for (SEXP v : value) {
      SET_VECTOR_ELT(attr, i++, v);
    }
    vector_.attr(name) = attr;
    UNPROTECT(1);
  }

  r_vector<r_string> names() const { return r_vector<r_string>(vector_.names()); }

  // Fast-path accessors for high-performance operations
  // These use the underlying r_vector's optimized data_ptr() methods
  CPP4R_ALWAYS_INLINE const underlying_type* CPP4R_RESTRICT data_ptr() const noexcept {
    return vector_.data_ptr();
  }

  // Writable data pointer - only available for writable matrices
  // Enable only if V has data_ptr_writable() method
  template <typename V2 = V>
  CPP4R_ALWAYS_INLINE underlying_type* CPP4R_RESTRICT data_ptr_writable() noexcept {
    return vector_.data_ptr_writable();
  }

  CPP4R_ALWAYS_INLINE T operator()(int row, int col) const {
    return vector_[row + (col * nrow())];
  }

  // Optimized operator() for writable matrices using direct pointer access when possible
  template <typename V2 = V, typename = decltype(std::declval<V2>().data_ptr_writable())>
  CPP4R_ALWAYS_INLINE typename V2::reference operator()(int row, int col) {
    return vector_[row + (col * nrow())];
  }

  slice operator[](int index) const { return {*this, index}; }

  slice_iterator begin() const { return {*this, 0}; }
  slice_iterator end() const { return {*this, nslices()}; }
};

template <typename S = by_column>
using doubles_matrix = matrix<r_vector<double>, double, S>;
template <typename S = by_column>
using integers_matrix = matrix<r_vector<int>, int, S>;
template <typename S = by_column>
using logicals_matrix = matrix<r_vector<r_bool>, r_bool, S>;
template <typename S = by_column>
using strings_matrix = matrix<r_vector<r_string>, r_string, S>;
template <typename S = by_column>
using complexes_matrix = matrix<r_vector<r_complex>, r_complex, S>;

namespace writable {
template <typename S = by_column>
using doubles_matrix = matrix<r_vector<double>, typename r_vector<double>::reference, S>;
template <typename S = by_column>
using integers_matrix = matrix<r_vector<int>, typename r_vector<int>::reference, S>;
template <typename S = by_column>
using logicals_matrix = matrix<r_vector<r_bool>, typename r_vector<r_bool>::reference, S>;
template <typename S = by_column>
using strings_matrix =
    matrix<r_vector<r_string>, typename r_vector<r_string>::reference, S>;
template <typename S = by_column>
using complexes_matrix =
    matrix<r_vector<r_complex>, typename r_vector<r_complex>::reference, S>;
}  // namespace writable

// Automatic coercion functions for matrices

// Coerce an integer or logical matrix to a doubles matrix
// This allows you to pass integer matrices where doubles matrices are expected
template <typename S = by_column>
inline doubles_matrix<S> as_doubles_matrix(SEXP x) {
  if (detail::r_typeof(x) == REALSXP) {
    return doubles_matrix<S>(x);
  } else if (detail::r_typeof(x) == INTSXP) {
    // Create a new doubles matrix with the same dimensions
    integers_matrix<S> xn(x);
    int nrow = xn.nrow();
    int ncol = xn.ncol();
    R_xlen_t size = static_cast<R_xlen_t>(nrow) * ncol;
    writable::doubles_matrix<S> ret(nrow, ncol);

    const int* CPP4R_RESTRICT x_ptr = INTEGER(xn.data());
    double* CPP4R_RESTRICT ret_ptr = REAL(ret.data());

#if CPP4R_HAS_CXX20
    // C++20: Use [[likely]] attribute for better optimization
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      if (val != NA_INTEGER) [[likely]] {
        ret_ptr[i] = static_cast<double>(val);
      } else {
        ret_ptr[i] = NA_REAL;
      }
    }
#elif CPP4R_HAS_CXX17
    // C++17: Compiler can better optimize with more context
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(val != NA_INTEGER) ? static_cast<double>(val) : NA_REAL;
    }
#else
    // C++11-14: Basic optimized loop
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(val != NA_INTEGER) ? static_cast<double>(val) : NA_REAL;
    }
#endif

    // Preserve attributes like dimnames
    SEXP dimnames = Rf_getAttrib(x, R_DimNamesSymbol);
    if (CPP4R_UNLIKELY(dimnames != R_NilValue)) {
      Rf_setAttrib(ret.data(), R_DimNamesSymbol, dimnames);
    }

    return ret;
  } else if (detail::r_typeof(x) == LGLSXP) {
    // Create a new doubles matrix from logical
    logicals_matrix<S> xn(x);
    int nrow = xn.nrow();
    int ncol = xn.ncol();
    R_xlen_t size = static_cast<R_xlen_t>(nrow) * ncol;
    writable::doubles_matrix<S> ret(nrow, ncol);

    const int* CPP4R_RESTRICT x_ptr = LOGICAL(xn.data());
    double* CPP4R_RESTRICT ret_ptr = REAL(ret.data());

#if CPP4R_HAS_CXX20
    // C++20: Use [[likely]] attribute for better optimization
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      if (val != NA_LOGICAL) [[likely]] {
        ret_ptr[i] = static_cast<double>(val);
      } else {
        ret_ptr[i] = NA_REAL;
      }
    }
#else
    // C++11-17: Use compiler builtin hints
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(val != NA_LOGICAL) ? static_cast<double>(val) : NA_REAL;
    }
#endif

    // Preserve dimnames
    SEXP dimnames = Rf_getAttrib(x, R_DimNamesSymbol);
    if (CPP4R_UNLIKELY(dimnames != R_NilValue)) {
      Rf_setAttrib(ret.data(), R_DimNamesSymbol, dimnames);
    }

    return ret;
  }

  throw type_error(REALSXP, detail::r_typeof(x));
}

// Coerce a doubles or logical matrix to an integers matrix
// Only works if all values are integer-like (no fractional parts)
template <typename S = by_column>
inline integers_matrix<S> as_integers_matrix(SEXP x) {
  if (detail::r_typeof(x) == INTSXP) {
    return integers_matrix<S>(x);
  } else if (detail::r_typeof(x) == REALSXP) {
    // Check if all values are integer-like before converting
    doubles_matrix<S> xn(x);
    int nrow = xn.nrow();
    int ncol = xn.ncol();
    R_xlen_t size = static_cast<R_xlen_t>(nrow) * ncol;

    const double* CPP4R_RESTRICT x_ptr = REAL(xn.data());

    // First pass: validate all values are integer-like
    for (R_xlen_t i = 0; i < size; ++i) {
      double val = x_ptr[i];
      if (CPP4R_UNLIKELY(!ISNA(val) && !is_convertible_without_loss_to_integer(val))) {
        throw std::runtime_error(
            "Cannot convert doubles matrix to integers: not all elements are "
            "integer-like");
      }
    }

    // Second pass: convert
    writable::integers_matrix<S> ret(nrow, ncol);
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

#if CPP4R_HAS_CXX20
    // C++20: Use [[likely]] attribute for better optimization
    for (R_xlen_t i = 0; i < size; ++i) {
      double val = x_ptr[i];
      if (!ISNA(val)) [[likely]] {
        ret_ptr[i] = static_cast<int>(val);
      } else {
        ret_ptr[i] = NA_INTEGER;
      }
    }
#else
    // C++11-17: Use compiler builtin hints
    for (R_xlen_t i = 0; i < size; ++i) {
      double val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(!ISNA(val)) ? static_cast<int>(val) : NA_INTEGER;
    }
#endif

    // Preserve dimnames
    SEXP dimnames = Rf_getAttrib(x, R_DimNamesSymbol);
    if (CPP4R_UNLIKELY(dimnames != R_NilValue)) {
      Rf_setAttrib(ret.data(), R_DimNamesSymbol, dimnames);
    }

    return ret;
  } else if (detail::r_typeof(x) == LGLSXP) {
    logicals_matrix<S> xn(x);
    int nrow = xn.nrow();
    int ncol = xn.ncol();
    R_xlen_t size = static_cast<R_xlen_t>(nrow) * ncol;
    writable::integers_matrix<S> ret(nrow, ncol);

    const int* CPP4R_RESTRICT x_ptr = LOGICAL(xn.data());
    int* CPP4R_RESTRICT ret_ptr = INTEGER(ret.data());

#if CPP4R_HAS_CXX20
    // C++20: Use [[likely]] attribute for better optimization
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      if (val != NA_LOGICAL) [[likely]] {
        ret_ptr[i] = val;
      } else {
        ret_ptr[i] = NA_INTEGER;
      }
    }
#else
    // C++11-17: Use compiler builtin hints
    for (R_xlen_t i = 0; i < size; ++i) {
      int val = x_ptr[i];
      ret_ptr[i] = CPP4R_LIKELY(val != NA_LOGICAL) ? val : NA_INTEGER;
    }
#endif

    // Preserve dimnames
    SEXP dimnames = Rf_getAttrib(x, R_DimNamesSymbol);
    if (CPP4R_UNLIKELY(dimnames != R_NilValue)) {
      Rf_setAttrib(ret.data(), R_DimNamesSymbol, dimnames);
    }

    return ret;
  }

  throw type_error(INTSXP, detail::r_typeof(x));
}

// TODO: Add tests for Matrix class
}  // namespace cpp4r
