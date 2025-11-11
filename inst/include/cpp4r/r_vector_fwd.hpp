#pragma once

#include <stddef.h>  // for ptrdiff_t, size_t

#include <algorithm>         // for max
#include <array>             // for array
#include <cstdio>            // for snprintf
#include <cstring>           // for memcpy
#include <exception>         // for exception
#include <initializer_list>  // for initializer_list
#include <iterator>          // for forward_iterator_tag, random_ac...
#include <stdexcept>         // for out_of_range
#include <string>            // for string, basic_string
#include <type_traits>       // for decay, is_same, enable_if, is_c...
#include <utility>           // for declval

#include "cpp4r/R.hpp"                // for R_xlen_t, SEXP, SEXPREC, Rf_xle...
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/named_arg.hpp"        // for named_arg
#include "cpp4r/protect.hpp"          // for store
#include "cpp4r/r_complex.hpp"        // for r_complex
#include "cpp4r/r_string.hpp"         // for r_string
#include "cpp4r/sexp.hpp"             // for sexp

namespace cpp4r {

using namespace cpp4r::literals;

namespace writable {
template <typename T>
class r_vector;
}  // namespace writable

// Declarations
template <typename T>
class r_vector {
 public:
  // Forward declare
  class const_iterator;
  using underlying_type = typename traits::get_underlying_type<T>::type;

 private:
  SEXP data_ = R_NilValue;
  SEXP protect_ = R_NilValue;
  bool is_altrep_ = false;
  underlying_type* data_p_ = nullptr;
  R_xlen_t length_ = 0;

 public:
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef T value_type;
  typedef T* pointer;
  typedef T& reference;

  ~r_vector();

  r_vector() noexcept = default;
  r_vector(SEXP data);
  r_vector(SEXP data, bool is_altrep);
  r_vector(const r_vector& x);
  r_vector(r_vector<T>&& x);
  r_vector(const writable::r_vector<T>& x);

  r_vector& operator=(const r_vector& rhs);
  r_vector& operator=(r_vector&& rhs);

  operator SEXP() const;
  operator sexp() const;

#ifdef LONG_VECTOR_SUPPORT
  T operator[](const int pos) const;
#endif
  T operator[](const R_xlen_t pos) const;
  T operator[](const size_type pos) const;
  T operator[](const r_string& name) const;

#ifdef LONG_VECTOR_SUPPORT
  T at(const int pos) const;
#endif
  T at(const R_xlen_t pos) const;
  T at(const size_type pos) const;
  T at(const r_string& name) const;

  bool contains(const r_string& name) const;
  bool is_altrep() const;
  bool named() const;
  R_xlen_t size() const;
  bool empty() const;
  SEXP data() const;

  const sexp attr(const char* name) const;
  const sexp attr(const std::string& name) const;
  const sexp attr(SEXP name) const;

  r_vector<r_string> names() const;

  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_iterator find(const r_string& name) const;

  class const_iterator {
    // Iterator references:
    // https://cplusplus.com/reference/iterator/
    // https://stackoverflow.com/questions/8054273/how-to-implement-an-stl-style-iterator-and-avoid-common-pitfalls
    // It seems like our iterator doesn't fully implement everything for
    // `random_access_iterator_tag` (like an `[]` operator, for example). If we discover
    // issues with it, we probably need to add more methods.
   private:
    const r_vector* data_;
    R_xlen_t pos_;
    std::array<underlying_type, 64 * 64> buf_;
    R_xlen_t block_start_ = 0;
    R_xlen_t length_ = 0;

   public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    const_iterator(const r_vector* data, R_xlen_t pos);

    const_iterator operator+(R_xlen_t pos);
    ptrdiff_t operator-(const const_iterator& other) const;

    const_iterator& operator++();
    const_iterator& operator--();

    const_iterator& operator+=(R_xlen_t pos);
    const_iterator& operator-=(R_xlen_t pos);

    bool operator!=(const const_iterator& other) const;
    bool operator==(const const_iterator& other) const;

    T operator*() const;

    friend class writable::r_vector<T>::iterator;

   private:
    /// Implemented in specialization
    static bool use_buf(bool is_altrep);
    void fill_buf(R_xlen_t pos);
  };

 private:
  /// Implemented in specialization
  static underlying_type get_elt(SEXP x, R_xlen_t i);
  /// Implemented in specialization
  static underlying_type* get_p(bool is_altrep, SEXP data);
  /// Implemented in specialization
  static underlying_type const* get_const_p(bool is_altrep, SEXP data);
  /// Implemented in specialization
  static void get_region(SEXP x, R_xlen_t i, R_xlen_t n, underlying_type* buf);
  /// Implemented in specialization
  static SEXPTYPE get_sexptype();
  /// Implemented in specialization (throws by default, specialization in list type)
  static T get_oob();
  static SEXP valid_type(SEXP x);
  static SEXP valid_length(SEXP x, R_xlen_t n);

  friend class writable::r_vector<T>;
};

namespace writable {

template <typename T>
using has_begin_fun = std::decay<decltype(*begin(std::declval<T>()))>;

/// Read/write access to new or copied r_vectors
template <typename T>
class r_vector : public cpp4r::r_vector<T> {
 public:
  // Forward declare
  class proxy;
  class iterator;

 private:
  R_xlen_t capacity_ = 0;

  using cpp4r::r_vector<T>::data_;
  using cpp4r::r_vector<T>::data_p_;
  using cpp4r::r_vector<T>::is_altrep_;
  using cpp4r::r_vector<T>::length_;
  using cpp4r::r_vector<T>::protect_;

  using typename cpp4r::r_vector<T>::underlying_type;

 public:
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef proxy value_type;
  typedef proxy* pointer;
  typedef proxy& reference;

  r_vector() noexcept = default;
  r_vector(const SEXP& data);
  r_vector(SEXP&& data);
  r_vector(const SEXP& data, bool is_altrep);
  r_vector(SEXP&& data, bool is_altrep);
  r_vector(const r_vector& rhs);
  r_vector(r_vector&& rhs);
  r_vector(const cpp4r::r_vector<T>& rhs);
  r_vector(std::initializer_list<T> il);
  explicit r_vector(std::initializer_list<named_arg> il);

  explicit r_vector(const R_xlen_t size);

  template <typename Iter>
  r_vector(Iter first, Iter last);

  template <typename V, typename W = has_begin_fun<V>>
  r_vector(const V& obj);

  r_vector& operator=(const r_vector& rhs);
  r_vector& operator=(r_vector&& rhs);

  operator SEXP() const;

#ifdef LONG_VECTOR_SUPPORT
  proxy operator[](const int pos) const;
#endif
  proxy operator[](const R_xlen_t pos) const;
  proxy operator[](const size_type pos) const;
  proxy operator[](const r_string& name) const;

#ifdef LONG_VECTOR_SUPPORT
  proxy at(const int pos) const;
#endif
  proxy at(const R_xlen_t pos) const;
  proxy at(const size_type pos) const;
  proxy at(const r_string& name) const;

  void push_back(T value);
  template <typename U = T,
            typename std::enable_if<std::is_same<U, r_string>::value>::type* = nullptr>
  void push_back(const std::string& value);  // Pacha: r_string only (#406)
  void push_back(const named_arg& value);
  void pop_back();

  void resize(R_xlen_t count);
  void reserve(R_xlen_t new_capacity);

  iterator insert(R_xlen_t pos, T value);
  iterator erase(R_xlen_t pos);

  void clear();

  iterator begin() const;
  iterator end() const;

  using cpp4r::r_vector<T>::cbegin;
  using cpp4r::r_vector<T>::cend;
  using cpp4r::r_vector<T>::size;

  iterator find(const r_string& name) const;

  /// Get the value at position without returning a proxy
  /// This is useful when you need the actual value (e.g., for C-style printf functions)
  /// that don't trigger implicit conversions from proxy types
#ifdef LONG_VECTOR_SUPPORT
  T value(const int pos) const;
#endif
  T value(const R_xlen_t pos) const;
  T value(const size_type pos) const;

  attribute_proxy<r_vector<T>> attr(const char* name) const;
  attribute_proxy<r_vector<T>> attr(const std::string& name) const;
  attribute_proxy<r_vector<T>> attr(SEXP name) const;

  attribute_proxy<r_vector<T>> names() const;

  class proxy {
   private:
    const SEXP data_;
    const R_xlen_t index_;
    underlying_type* const p_;
    bool is_altrep_;

   public:
    proxy(SEXP data, const R_xlen_t index, underlying_type* const p, bool is_altrep);

    proxy& operator=(const proxy& rhs);

    proxy& operator=(const T& rhs);
    
    template <typename U>
    proxy& operator=(const U& rhs);
    
    proxy& operator+=(const T& rhs);
    proxy& operator-=(const T& rhs);
    proxy& operator*=(const T& rhs);
    proxy& operator/=(const T& rhs);
    proxy& operator++(int);
    proxy& operator--(int);

    void operator++();
    void operator--();

    operator T() const;

   private:
    underlying_type get() const;
    void set(underlying_type x);
  };

  class iterator : public cpp4r::r_vector<T>::const_iterator {
   private:
    using cpp4r::r_vector<T>::const_iterator::data_;
    using cpp4r::r_vector<T>::const_iterator::block_start_;
    using cpp4r::r_vector<T>::const_iterator::pos_;
    using cpp4r::r_vector<T>::const_iterator::buf_;
    using cpp4r::r_vector<T>::const_iterator::length_;
    using cpp4r::r_vector<T>::const_iterator::use_buf;
    using cpp4r::r_vector<T>::const_iterator::fill_buf;

   public:
    using difference_type = ptrdiff_t;
    using value_type = proxy;
    using pointer = proxy*;
    using reference = proxy&;
    using iterator_category = std::forward_iterator_tag;

    iterator(const r_vector* data, R_xlen_t pos);

    iterator& operator++();

    proxy operator*() const;

    using cpp4r::r_vector<T>::const_iterator::operator!=;

    iterator& operator+=(R_xlen_t rhs);
    iterator operator+(R_xlen_t rhs);
  };

 private:
  /// Implemented in specialization
  static void set_elt(SEXP x, R_xlen_t i, underlying_type value);

  static SEXP reserve_data(SEXP x, bool is_altrep, R_xlen_t size);
  static SEXP resize_data(SEXP x, bool is_altrep, R_xlen_t size);
  static SEXP resize_names(SEXP x, R_xlen_t size);

  using cpp4r::r_vector<T>::get_elt;
  using cpp4r::r_vector<T>::get_p;
  using cpp4r::r_vector<T>::get_const_p;
  using cpp4r::r_vector<T>::get_sexptype;
  using cpp4r::r_vector<T>::valid_type;
  using cpp4r::r_vector<T>::valid_length;
};
}  // namespace writable

class type_error : public std::exception {
 public:
  type_error(SEXPTYPE expected, SEXPTYPE actual) : expected_(expected), actual_(actual) {}
  virtual const char* what() const noexcept override {
    snprintf(str_, 64, "Invalid input type, expected '%s' actual '%s'",
             Rf_type2char(expected_), Rf_type2char(actual_));
    return str_;
  }

 private:
  SEXPTYPE expected_;
  SEXPTYPE actual_;
  mutable char str_[64];
};

// Helper type traits for as_cpp conversions
template <typename C, typename T = typename std::decay<C>::type::value_type>
using is_vector_of_strings = typename std::enable_if<
    std::is_same<typename std::decay<T>::type, std::string>::value,
    typename std::decay<C>::type>::type;

}  // namespace cpp4r
