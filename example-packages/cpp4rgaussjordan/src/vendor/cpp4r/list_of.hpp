#pragma once

#include <string>  // for string, basic_string

#include "cpp4r/R.hpp"     // for R_xlen_t, SEXP, SEXPREC, LONG_VECTOR_SUPPORT
#include "cpp4r/list.hpp"  // for list

namespace cpp4r {

template <typename T>
class list_of : public list {
 public:
  inline list_of(const list& data) noexcept : list(data) {}

#ifdef LONG_VECTOR_SUPPORT
  inline T operator[](const int pos) const {
    return operator[](static_cast<R_xlen_t>(pos));
  }
#endif

  inline T operator[](const R_xlen_t pos) const { return list::operator[](pos); }

  inline T operator[](const char* pos) const { return list::operator[](pos); }

  inline T operator[](const std::string& pos) const {
    return list::operator[](pos.c_str());
  }
};

namespace writable {
template <typename T>
class list_of : public writable::list {
 public:
  inline list_of(const list& data) : writable::list(data) {}
  inline explicit list_of(R_xlen_t n) : writable::list(n) {}

  class proxy {
   private:
    writable::list::proxy data_;

   public:
    inline proxy(const writable::list::proxy& data) noexcept : data_(data) {}

    inline operator T() const { return static_cast<SEXP>(*this); }
    inline operator SEXP() const noexcept { return static_cast<SEXP>(data_); }
#ifdef LONG_VECTOR_SUPPORT
    inline typename T::proxy operator[](int pos) { return static_cast<T>(data_)[pos]; }
#endif
    inline typename T::proxy operator[](R_xlen_t pos) {
      return static_cast<T>(data_)[pos];
    }
    inline typename T::proxy operator[](const char* pos) {
      return static_cast<T>(data_)[pos];
    }
    inline typename T::proxy operator[](const std::string& pos) {
      return static_cast<T>(data_)[pos.c_str()];
    }
    inline proxy& operator=(const T& rhs) {
      data_ = rhs;
      return *this;
    }
  };

#ifdef LONG_VECTOR_SUPPORT
  inline proxy operator[](int pos) {
    return {writable::list::operator[](static_cast<R_xlen_t>(pos))};
  }
#endif

  inline proxy operator[](R_xlen_t pos) { return {writable::list::operator[](pos)}; }

  inline proxy operator[](const char* pos) { return {writable::list::operator[](pos)}; }

  inline proxy operator[](const std::string& pos) {
    return {writable::list::operator[](pos.c_str())};
  }
};
}  // namespace writable

}  // namespace cpp4r
