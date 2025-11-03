#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <initializer_list>  // for initializer_list
#include <iterator>
#include <string>  // for string

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#include "cpp4r/R.hpp"          // for SEXP, SEXPREC, R_xlen_t, INT...
#include "cpp4r/r_bool.hpp"     // for r_bool
#include "cpp4r/r_complex.hpp"  // for r_complex
#include "cpp4r/r_string.hpp"   // for r_string
#include "cpp4r/r_vector.hpp"   // for r_vector
#include "cpp4r/sexp.hpp"       // for sexp

namespace cpp4r {

// matrix dimensions
struct matrix_dims {
 protected:
  const int nrow_;
  const int ncol_;

 public:
  matrix_dims(SEXP data) : nrow_(Rf_nrows(data)), ncol_(Rf_ncols(data)) {}
  matrix_dims(int nrow, int ncol) : nrow_(nrow), ncol_(ncol) {}

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD int nrow() const { return nrow_; }
  CPP4R_NODISCARD int ncol() const { return ncol_; }
#else
  int nrow() const { return nrow_; }
  int ncol() const { return ncol_; }
#endif
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

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD int nslices() const { return nrow(); }
  CPP4R_NODISCARD int slice_size() const { return ncol(); }
  CPP4R_NODISCARD int slice_stride() const { return nrow(); }
  CPP4R_NODISCARD int slice_offset(int pos) const { return pos; }
#else
  int nslices() const { return nrow(); }
  int slice_size() const { return ncol(); }
  int slice_stride() const { return nrow(); }
  int slice_offset(int pos) const { return pos; }
#endif
};

// basic properties of matrix column slices
template <>
struct matrix_slices<by_column> : public matrix_dims {
 public:
  using matrix_dims::matrix_dims;
  using matrix_dims::ncol;
  using matrix_dims::nrow;

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD int nslices() const { return ncol(); }
  CPP4R_NODISCARD int slice_size() const { return nrow(); }
  CPP4R_NODISCARD int slice_stride() const { return 1; }
  CPP4R_NODISCARD int slice_offset(int pos) const { return pos * nrow(); }
#else
  int nslices() const { return ncol(); }
  int slice_size() const { return nrow(); }
  int slice_stride() const { return 1; }
  int slice_offset(int pos) const { return pos * nrow(); }
#endif
};

template <typename V, typename T, typename S = by_column>
class matrix : public matrix_slices<S> {
 private:
  V vector_;

 public:
  // matrix slice: row (if S=by_row) or a column (if S=by_column)
  class slice {
   private:
    const matrix& parent_;
    int index_;   // slice index
    int offset_;  // index of the first slice element in parent_.vector_

   public:
    slice(const matrix& parent, int index)
        : parent_(parent), index_(index), offset_(parent.slice_offset(index)) {}

#if CPP4R_HAS_CXX17
    CPP4R_NODISCARD R_xlen_t stride() const noexcept { return parent_.slice_stride(); }
    CPP4R_NODISCARD R_xlen_t size() const noexcept { return parent_.slice_size(); }
#else
    R_xlen_t stride() const noexcept { return parent_.slice_stride(); }
    R_xlen_t size() const noexcept { return parent_.slice_size(); }
#endif

    bool operator==(const slice& rhs) const noexcept {
#if CPP4R_HAS_CXX20
      return (index_ == rhs.index_) && (parent_.data() == rhs.parent_.data()) CPP4R_LIKELY;
#else
      return (index_ == rhs.index_) && (parent_.data() == rhs.parent_.data());
#endif
    }
    bool operator!=(const slice& rhs) const noexcept { return !operator==(rhs); }

#if CPP4R_HAS_CXX17
    CPP4R_NODISCARD T operator[](int pos) const noexcept {
      return parent_.vector_[offset_ + stride() * pos];
    }
#else
    T operator[](int pos) const noexcept {
      return parent_.vector_[offset_ + stride() * pos];
    }
#endif

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

      iterator& operator++() noexcept {
        ++pos_;
        return *this;
      }

      bool operator==(const iterator& rhs) const noexcept {
#if CPP4R_HAS_CXX20
        return (pos_ == rhs.pos_) && (slice_ == rhs.slice_) CPP4R_LIKELY;
#else
        return (pos_ == rhs.pos_) && (slice_ == rhs.slice_);
#endif
      }
      bool operator!=(const iterator& rhs) const noexcept { return !operator==(rhs); }

#if CPP4R_HAS_CXX17
      CPP4R_NODISCARD T operator*() const noexcept { return slice_[pos_]; };
#else
      T operator*() const noexcept { return slice_[pos_]; };
#endif
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

    slice_iterator& operator++() noexcept {
      ++pos_;
      return *this;
    }

    bool operator==(const slice_iterator& rhs) const noexcept {
#if CPP4R_HAS_CXX20
      return (pos_ == rhs.pos_) && (parent_.data() == rhs.parent_.data()) CPP4R_LIKELY;
#else
      return (pos_ == rhs.pos_) && (parent_.data() == rhs.parent_.data());
#endif
    }
    bool operator!=(const slice_iterator& rhs) const noexcept { return !operator==(rhs); }

#if CPP4R_HAS_CXX17
    CPP4R_NODISCARD slice operator*() { return parent_[pos_]; };
#else
    slice operator*() { return parent_[pos_]; };
#endif
  };

 public:
  matrix(SEXP data) : matrix_slices<S>(data), vector_(data) {}

  template <typename V2, typename T2, typename S2>
  matrix(const cpp4r::matrix<V2, T2, S2>& rhs)
      : matrix_slices<S>(rhs.nrow(), rhs.ncol()), vector_(rhs.vector()) {}

  matrix(int nrow, int ncol)
      : matrix_slices<S>(nrow, ncol), vector_(R_xlen_t(nrow * ncol)) {
    vector_.attr(R_DimSymbol) = {nrow, ncol};
  }

  // Copy constructor
  matrix(const matrix& other)
      : matrix_slices<S>(other.nrow(), other.ncol()), vector_(other.vector_) {}

  // Copy assignment
  matrix& operator=(const matrix& other) {
    if (this != &other) {
      vector_ = other.vector_;
      // Note: matrix_slices dimensions are recalculated from vector
    }
    return *this;
  }

  // Move constructor
  matrix(matrix&& other) noexcept
      : matrix_slices<S>(other.nrow(), other.ncol()), vector_(std::move(other.vector_)) {}

  // Move assignment
  matrix& operator=(matrix&& other) noexcept {
    if (this != &other) {
      vector_ = std::move(other.vector_);
      // Note: matrix_slices is not copyable/moveable, but dimensions are recalculated
      // from vector
    }
    return *this;
  }

  using matrix_slices<S>::nrow;
  using matrix_slices<S>::ncol;
  using matrix_slices<S>::nslices;
  using matrix_slices<S>::slice_size;
  using matrix_slices<S>::slice_stride;
  using matrix_slices<S>::slice_offset;

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD V vector() const { return vector_; }

  CPP4R_NODISCARD SEXP data() const { return vector_.data(); }

  CPP4R_NODISCARD R_xlen_t size() const { return vector_.size(); }

  CPP4R_NODISCARD operator SEXP() const { return SEXP(vector_); }
#else
  V vector() const { return vector_; }

  SEXP data() const { return vector_.data(); }

  R_xlen_t size() const { return vector_.size(); }

  operator SEXP() const { return SEXP(vector_); }
#endif

  // operator sexp() { return sexp(vector_); }

  attribute_proxy<V> attr(const char* name) { return attribute_proxy<V>(vector_, name); }

  attribute_proxy<V> attr(const std::string& name) {
    return attribute_proxy<V>(vector_, name.c_str());
  }

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload
  attribute_proxy<V> attr(std::string_view name) {
    return attribute_proxy<V>(vector_, name.data());
  }
#endif

  attribute_proxy<V> attr(SEXP name) { return attribute_proxy<V>(vector_, name); }

  void attr(const char* name, SEXP value) { vector_.attr(name) = value; }

  void attr(const std::string& name, SEXP value) { vector_.attr(name) = value; }

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload
  void attr(std::string_view name, SEXP value) { vector_.attr(name.data()) = value; }
#endif

  void attr(SEXP name, SEXP value) { vector_.attr(name) = value; }

 private:
  SEXP create_sexp_list(std::initializer_list<SEXP> value) const {
    SEXP attr = PROTECT(Rf_allocVector(VECSXP, value.size()));
    int i = 0;
    for (SEXP v : value) {
      SET_VECTOR_ELT(attr, i++, v);
    }
    UNPROTECT(1);
    return attr;
  }

 public:
  void attr(const char* name, std::initializer_list<SEXP> value) {
    vector_.attr(name) = create_sexp_list(value);
  }

  void attr(const std::string& name, std::initializer_list<SEXP> value) {
    vector_.attr(name) = create_sexp_list(value);
  }

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload
  void attr(std::string_view name, std::initializer_list<SEXP> value) {
    vector_.attr(name.data()) = create_sexp_list(value);
  }
#endif

  void attr(SEXP name, std::initializer_list<SEXP> value) {
    vector_.attr(name) = create_sexp_list(value);
  }

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD r_vector<r_string> names() const { return r_vector<r_string>(vector_.names()); }

  CPP4R_NODISCARD T operator()(int row, int col) const noexcept { return vector_[row + (col * nrow())]; }

  CPP4R_NODISCARD slice operator[](int index) const { return {*this, index}; }
#else
  r_vector<r_string> names() const { return r_vector<r_string>(vector_.names()); }

  T operator()(int row, int col) const noexcept { return vector_[row + (col * nrow())]; }

  slice operator[](int index) const { return {*this, index}; }
#endif

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
using doubles_matrix = matrix<r_vector<double>, r_vector<double>::proxy, S>;
template <typename S = by_column>
using integers_matrix = matrix<r_vector<int>, r_vector<int>::proxy, S>;
template <typename S = by_column>
using logicals_matrix = matrix<r_vector<r_bool>, r_vector<r_bool>::proxy, S>;
template <typename S = by_column>
using strings_matrix = matrix<r_vector<r_string>, r_vector<r_string>::proxy, S>;
template <typename S = by_column>
using complexes_matrix = matrix<r_vector<r_complex>, r_vector<r_complex>::proxy, S>;
}  // namespace writable

// TODO: Add tests for Matrix class
}  // namespace cpp4r
