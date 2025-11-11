#pragma once

#include "cpp4r/r_vector_fwd.hpp"

namespace cpp4r {

// Read-only r_vector implementations

template <typename T>
inline r_vector<T>::~r_vector() {
  detail::store::release(protect_);
}

template <typename T>
inline r_vector<T>::r_vector(const SEXP data)
    : data_(valid_type(data)),
      protect_(detail::store::insert(data)),
      is_altrep_(ALTREP(data)),
      data_p_(get_p(ALTREP(data), data)),
      length_(Rf_xlength(data)) {}

template <typename T>
inline r_vector<T>::r_vector(const SEXP data, bool is_altrep)
    : data_(valid_type(data)),
      protect_(detail::store::insert(data)),
      is_altrep_(is_altrep),
      data_p_(get_p(is_altrep, data)),
      length_(Rf_xlength(data)) {}

// We are in read-only space so we can just copy over all properties except for
// `protect_`, which we need to manage on our own. `x` persists after this call, so we
// don't clear anything.
template <typename T>
inline r_vector<T>::r_vector(const r_vector& x) {
  data_ = x.data_;
  protect_ = detail::store::insert(data_);
  is_altrep_ = x.is_altrep_;
  data_p_ = x.data_p_;
  length_ = x.length_;
}

// `x` here is a temporary value, it is going to be destructed right after this.
// Take ownership over all `x` details, including `protect_`.
// Importantly, set `x.protect_` to `R_NilValue` to prevent the `x` destructor from
// releasing the object that we now own.
template <typename T>
inline r_vector<T>::r_vector(r_vector&& x) {
  data_ = x.data_;
  protect_ = x.protect_;
  is_altrep_ = x.is_altrep_;
  data_p_ = x.data_p_;
  length_ = x.length_;

  // Important for `x.protect_`, extra check for everything else
  x.data_ = R_NilValue;
  x.protect_ = R_NilValue;
  x.is_altrep_ = false;
  x.data_p_ = nullptr;
  x.length_ = 0;
}

// `x` here is writable, meaning the underlying `SEXP` could have more `capacity` than
// a read only equivalent would expect. This means we have to go through `SEXP` first,
// to truncate the writable data, and then we can wrap it in a read only `r_vector`.
//
// It would be the same scenario if we came from a writable temporary, i.e.
// `writable::r_vector<T>&& x`, so we let this method handle both scenarios.
template <typename T>
inline r_vector<T>::r_vector(const writable::r_vector<T>& x)
    : r_vector(static_cast<SEXP>(x)) {}

// Same reasoning as `r_vector(const r_vector& x)` constructor
template <typename T>
inline r_vector<T>& r_vector<T>::operator=(const r_vector& rhs) {
  if (data_ == rhs.data_) {
    return *this;
  }

  // Release existing object that we protect
  detail::store::release(protect_);

  data_ = rhs.data_;
  protect_ = detail::store::insert(data_);
  is_altrep_ = rhs.is_altrep_;
  data_p_ = rhs.data_p_;
  length_ = rhs.length_;

  return *this;
}

// Same reasoning as `r_vector(r_vector&& x)` constructor
template <typename T>
inline r_vector<T>& r_vector<T>::operator=(r_vector&& rhs) {
  if (data_ == rhs.data_) {
    return *this;
  }

  // Release existing object that we protect
  detail::store::release(protect_);

  data_ = rhs.data_;
  protect_ = rhs.protect_;
  is_altrep_ = rhs.is_altrep_;
  data_p_ = rhs.data_p_;
  length_ = rhs.length_;

  // Important for `rhs.protect_`, extra check for everything else
  rhs.data_ = R_NilValue;
  rhs.protect_ = R_NilValue;
  rhs.is_altrep_ = false;
  rhs.data_p_ = nullptr;
  rhs.length_ = 0;

  return *this;
}

template <typename T>
inline r_vector<T>::operator SEXP() const {
  return data_;
}

template <typename T>
inline r_vector<T>::operator sexp() const {
  return data_;
}

#ifdef LONG_VECTOR_SUPPORT
template <typename T>
inline T r_vector<T>::operator[](const int pos) const {
  return operator[](static_cast<R_xlen_t>(pos));
}
#endif

template <typename T>
inline T r_vector<T>::operator[](const R_xlen_t pos) const {
  // Handles ALTREP, VECSXP, and STRSXP cases through `get_elt()`
  const underlying_type elt = (data_p_ != nullptr) ? data_p_[pos] : get_elt(data_, pos);
  return static_cast<T>(elt);
}

template <typename T>
inline T r_vector<T>::operator[](const size_type pos) const {
  return operator[](static_cast<R_xlen_t>(pos));
}

template <typename T>
inline T r_vector<T>::operator[](const r_string& name) const {
  SEXP names = this->names();
  R_xlen_t size = Rf_xlength(names);

  for (R_xlen_t pos = 0; pos < size; ++pos) {
    auto cur = Rf_translateCharUTF8(STRING_ELT(names, pos));
    if (name == cur) {
      return operator[](pos);
    }
  }

  return get_oob();
}

#ifdef LONG_VECTOR_SUPPORT
template <typename T>
inline T r_vector<T>::at(const int pos) const {
  return at(static_cast<R_xlen_t>(pos));
}
#endif

template <typename T>
inline T r_vector<T>::at(const R_xlen_t pos) const {
  if (pos < 0 || pos >= length_) {
    throw std::out_of_range("r_vector");
  }

  return operator[](pos);
}

template <typename T>
inline T r_vector<T>::at(const size_type pos) const {
  return at(static_cast<R_xlen_t>(pos));
}

template <typename T>
inline T r_vector<T>::at(const r_string& name) const {
  return operator[](name);
}

template <typename T>
inline bool r_vector<T>::contains(const r_string& name) const {
  SEXP names = this->names();
  R_xlen_t size = Rf_xlength(names);

  for (R_xlen_t pos = 0; pos < size; ++pos) {
    auto cur = Rf_translateCharUTF8(STRING_ELT(names, pos));
    if (name == cur) {
      return true;
    }
  }

  return false;
}

template <typename T>
inline bool r_vector<T>::is_altrep() const {
  return is_altrep_;
}

template <typename T>
inline bool r_vector<T>::named() const {
  return Rf_getAttrib(data_, R_NamesSymbol) != R_NilValue;
}

template <typename T>
inline R_xlen_t r_vector<T>::size() const {
  return length_;
}

template <typename T>
inline bool r_vector<T>::empty() const {
  return (!(this->size() > 0));
}

/// Provide access to the underlying data, mainly for interface
/// compatibility with std::vector
template <typename T>
inline SEXP r_vector<T>::data() const {
  return data_;
}

template <typename T>
inline const sexp r_vector<T>::attr(const char* name) const {
  return SEXP(attribute_proxy<r_vector<T>>(*this, name));
}

template <typename T>
inline const sexp r_vector<T>::attr(const std::string& name) const {
  return SEXP(attribute_proxy<r_vector<T>>(*this, name.c_str()));
}

template <typename T>
inline const sexp r_vector<T>::attr(SEXP name) const {
  return SEXP(attribute_proxy<r_vector<T>>(*this, name));
}

template <typename T>
inline r_vector<r_string> r_vector<T>::names() const {
  SEXP nms = Rf_getAttrib(data_, R_NamesSymbol);
  if (nms == R_NilValue) {
    return r_vector<r_string>();
  } else {
    return r_vector<r_string>(nms);
  }
}

template <typename T>
inline T r_vector<T>::get_oob() {
  throw std::out_of_range("r_vector");
}

template <typename T>
inline SEXP r_vector<T>::valid_type(SEXP x) {
  const SEXPTYPE type = get_sexptype();

  if (x == nullptr) {
    throw type_error(type, NILSXP);
  }
  if (detail::r_typeof(x) != type) {
    throw type_error(type, detail::r_typeof(x));
  }

  return x;
}

template <typename T>
inline SEXP r_vector<T>::valid_length(SEXP x, R_xlen_t n) {
  R_xlen_t x_n = Rf_xlength(x);

  if (x_n == n) {
    return x;
  }

  char message[128];
  snprintf(message, 128,
           "Invalid input length, expected '%" cpp4r_PRIdXLEN_T
           "' actual '%" cpp4r_PRIdXLEN_T "'.",
           n, x_n);

  throw std::length_error(message);
}

template <typename T>
inline typename r_vector<T>::const_iterator r_vector<T>::begin() const {
  return const_iterator(this, 0);
}

template <typename T>
inline typename r_vector<T>::const_iterator r_vector<T>::end() const {
  return const_iterator(this, length_);
}

template <typename T>
inline typename r_vector<T>::const_iterator r_vector<T>::cbegin() const {
  return const_iterator(this, 0);
}

template <typename T>
inline typename r_vector<T>::const_iterator r_vector<T>::cend() const {
  return const_iterator(this, length_);
}

template <typename T>
r_vector<T>::const_iterator::const_iterator(const r_vector* data, R_xlen_t pos)
    : data_(data), pos_(pos), buf_() {
  if (use_buf(data_->is_altrep())) {
    fill_buf(pos);
  }
}

template <typename T>
inline typename r_vector<T>::const_iterator& r_vector<T>::const_iterator::operator++() {
  ++pos_;
  if (use_buf(data_->is_altrep()) && pos_ >= block_start_ + length_) {
    fill_buf(pos_);
  }
  return *this;
}

template <typename T>
inline typename r_vector<T>::const_iterator& r_vector<T>::const_iterator::operator--() {
  --pos_;
  if (use_buf(data_->is_altrep()) && pos_ > 0 && pos_ < block_start_) {
    fill_buf(std::max(0_xl, pos_ - 64));
  }
  return *this;
}

template <typename T>
inline typename r_vector<T>::const_iterator& r_vector<T>::const_iterator::operator+=(
    R_xlen_t i) {
  pos_ += i;
  if (use_buf(data_->is_altrep()) && pos_ >= block_start_ + length_) {
    fill_buf(pos_);
  }
  return *this;
}

template <typename T>
inline typename r_vector<T>::const_iterator& r_vector<T>::const_iterator::operator-=(
    R_xlen_t i) {
  pos_ -= i;
  if (use_buf(data_->is_altrep()) && pos_ >= block_start_ + length_) {
    fill_buf(std::max(0_xl, pos_ - 64));
  }
  return *this;
}

template <typename T>
inline bool r_vector<T>::const_iterator::operator!=(
    const r_vector::const_iterator& other) const {
  return pos_ != other.pos_;
}

template <typename T>
inline bool r_vector<T>::const_iterator::operator==(
    const r_vector::const_iterator& other) const {
  return pos_ == other.pos_;
}

template <typename T>
inline ptrdiff_t r_vector<T>::const_iterator::operator-(
    const r_vector::const_iterator& other) const {
  return pos_ - other.pos_;
}

template <typename T>
inline typename r_vector<T>::const_iterator r_vector<T>::const_iterator::operator+(
    R_xlen_t rhs) {
  auto it = *this;
  it += rhs;
  return it;
}

template <typename T>
inline typename r_vector<T>::const_iterator r_vector<T>::find(
    const r_string& name) const {
  SEXP names = this->names();
  R_xlen_t size = Rf_xlength(names);

  for (R_xlen_t pos = 0; pos < size; ++pos) {
    auto cur = Rf_translateCharUTF8(STRING_ELT(names, pos));
    if (name == cur) {
      return begin() + pos;
    }
  }

  return end();
}

template <typename T>
inline T r_vector<T>::const_iterator::operator*() const {
  if (use_buf(data_->is_altrep())) {
    // Use pre-loaded buffer for compatible ALTREP types
    return static_cast<T>(buf_[pos_ - block_start_]);
  } else {
    // Otherwise pass through to normal retrieval method
    return data_->operator[](pos_);
  }
}

template <typename T>
inline void r_vector<T>::const_iterator::fill_buf(R_xlen_t pos) {
  using namespace cpp4r::literals;
  length_ = std::min(64_xl, data_->size() - pos);
  get_region(data_->data_, pos, length_, buf_.data());
  block_start_ = pos;
}

}  // namespace cpp4r
