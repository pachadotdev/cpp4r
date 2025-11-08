#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <string>  // for string, basic_string

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#include "cpp4r/R.hpp"     // for R_xlen_t, SEXP, SEXPREC, LONG_VECTOR_SUPPORT
#include "cpp4r/list.hpp"  // for list

namespace cpp4r {

template <typename T>
class list_of : public list {
 public:
  inline list_of(const list& data) noexcept : list(data) {}

#ifdef LONG_VECTOR_SUPPORT
#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD inline T operator[](const int pos) const {
    return operator[](static_cast<R_xlen_t>(pos));
  }
#else
  inline T operator[](const int pos) const {
    return operator[](static_cast<R_xlen_t>(pos));
  }
#endif
#endif

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD inline T operator[](const R_xlen_t pos) const {
    return list::operator[](pos);
  }

  CPP4R_NODISCARD inline T operator[](const char* pos) const {
    return list::operator[](pos);
  }

  CPP4R_NODISCARD inline T operator[](const std::string& pos) const {
    return list::operator[](pos.c_str());
  }

  // C++17: Add string_view overload
  CPP4R_NODISCARD inline T operator[](std::string_view pos) const {
    return list::operator[](pos.data());
  }
#else
  inline T operator[](const R_xlen_t pos) const { return list::operator[](pos); }

  inline T operator[](const char* pos) const { return list::operator[](pos); }

  inline T operator[](const std::string& pos) const {
    return list::operator[](pos.c_str());
  }
#endif
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

#if CPP4R_HAS_CXX17
    CPP4R_NODISCARD inline operator T() const { return static_cast<SEXP>(*this); }
    CPP4R_NODISCARD inline operator SEXP() const noexcept {
      return static_cast<SEXP>(data_);
    }
#else
    inline operator T() const { return static_cast<SEXP>(*this); }
    inline operator SEXP() const noexcept { return static_cast<SEXP>(data_); }
#endif

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

#if CPP4R_HAS_CXX17
    // C++17: Add string_view overload
    inline typename T::proxy operator[](std::string_view pos) {
      return static_cast<T>(data_)[pos.data()];
    }
#endif

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

#if CPP4R_HAS_CXX17
  // C++17: Add string_view overload
  inline proxy operator[](std::string_view pos) {
    return {writable::list::operator[](pos.data())};
  }
#endif
};
}  // namespace writable

}  // namespace cpp4r
