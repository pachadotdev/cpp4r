#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <cstddef>      // for nullptr_t, NULL
#include <memory>       // for bad_weak_ptr
#include <type_traits>  // for add_lvalue_reference

#include "cpp4r/R.hpp"         // for SEXP, SEXPREC, R_NilValue
#include "cpp4r/protect.hpp"   // for protect, safe, protect::function
#include "cpp4r/r_bool.hpp"    // for r_bool
#include "cpp4r/r_vector.hpp"  // for type_error
#include "cpp4r/sexp.hpp"      // for sexp

namespace cpp4r {

template <typename T>
void default_deleter(T* obj) {
  delete obj;
}

template <typename T, void Deleter(T*) = default_deleter<T>>
class external_pointer {
 private:
  sexp data_ = R_NilValue;

  static SEXP valid_type(SEXP data) {
    // Pacha: Allow nullable external_pointer (#312)
    if (data == R_NilValue) {
      return data;
    }

    // Cache the type check to avoid multiple calls
    SEXPTYPE data_type = detail::r_typeof(data);
    if (data_type != EXTPTRSXP) {
      throw type_error(EXTPTRSXP, data_type);
    }

    return data;
  }

  static void r_deleter(SEXP p) {
    if (detail::r_typeof(p) != EXTPTRSXP) {
      return;
    }

    T* ptr = static_cast<T*>(R_ExternalPtrAddr(p));

    if (ptr == nullptr) {
      return;
    }

    R_ClearExternalPtr(p);

    Deleter(ptr);
  }

 public:
  using pointer = T*;

  external_pointer() noexcept {}
  external_pointer(std::nullptr_t) noexcept {}

  external_pointer(SEXP data) : data_(valid_type(data)) {}

  external_pointer(pointer p, bool use_deleter = true, bool finalize_on_exit = true)
      : data_(safe[R_MakeExternalPtr]((void*)p, R_NilValue, R_NilValue)) {
    if (use_deleter) {
      R_RegisterCFinalizerEx(data_, r_deleter, static_cast<r_bool>(finalize_on_exit));
    }
  }

  external_pointer(const external_pointer& rhs) {
    data_ = safe[Rf_shallow_duplicate](rhs.data_);
  }

  external_pointer& operator=(const external_pointer& rhs) {
    if (this != &rhs) {
      data_ = safe[Rf_shallow_duplicate](rhs.data_);
    }
    return *this;
  }

  // the old external_pointer(external_pointer&& rhs) { reset(rhs.release()); }
  // affects duckdb [@krlmlr, r-lib/cpp11/pull/423/files]
  external_pointer(external_pointer&& rhs) noexcept {
    data_ = rhs.data_;
    rhs.data_ = R_NilValue;
  }

  // same for the old external_pointer& operator=(external_pointer&& rhs) noexcept {
  // reset(rhs.release()); }
  external_pointer& operator=(external_pointer&& rhs) noexcept {
    if (this != &rhs) {
      data_ = rhs.data_;
      rhs.data_ = R_NilValue;
    }
    return *this;
  }

  external_pointer& operator=(std::nullptr_t) noexcept {
    reset();
    return *this;
  }

#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD operator SEXP() const noexcept { return data_; }

  CPP4R_NODISCARD pointer get() const noexcept {
    pointer addr = static_cast<T*>(R_ExternalPtrAddr(data_));
    return addr;  // No need to check for nullptr twice
  }
#else
  operator SEXP() const noexcept { return data_; }

  pointer get() const noexcept {
    pointer addr = static_cast<T*>(R_ExternalPtrAddr(data_));
    return addr;  // No need to check for nullptr twice
  }
#endif

#if CPP4R_HAS_CXX14
  typename std::add_lvalue_reference<T>::type operator*() {
    pointer addr = get();
    if (addr == nullptr) {
      throw std::bad_weak_ptr();
    }
    return *addr;  // Use cached addr instead of calling get() again
  }
#else
  typename std::add_lvalue_reference<T>::type operator*() {
    pointer addr = get();
    if (addr == nullptr) {
      throw std::bad_weak_ptr();
    }
    return *addr;  // Use cached addr instead of calling get() again
  }
#endif

  pointer operator->() const {
    pointer addr = get();
    if (addr == nullptr) {
      throw std::bad_weak_ptr();
    }
    return addr;  // Use cached addr instead of calling get() again
  }

  pointer release() noexcept {
    pointer ptr = get();
    if (ptr == nullptr) {
      return nullptr;
    }
    R_ClearExternalPtr(data_);

    return ptr;
  }

  void reset(pointer ptr = pointer()) {
    SEXP old_data = data_;
    if (ptr != nullptr) {
      data_ = safe[R_MakeExternalPtr]((void*)ptr, R_NilValue, R_NilValue);
    } else {
      data_ = R_NilValue;
    }
    // Clean up old data if it was an external pointer
    if (old_data != R_NilValue) {
      r_deleter(old_data);
    }
  }

  void swap(external_pointer& other) noexcept {
    SEXP tmp = other.data_;
    other.data_ = data_;
    data_ = tmp;
  }

  // Pacha: Support nullable external_pointer (#312)
#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD operator bool() const noexcept { return data_ != R_NilValue; }
#else
  operator bool() const noexcept { return data_ != R_NilValue; }
#endif
};

template <class T, void Deleter(T*)>
void swap(external_pointer<T, Deleter>& lhs, external_pointer<T, Deleter>& rhs) noexcept {
  lhs.swap(rhs);
}

template <class T, void Deleter(T*)>
bool operator==(const external_pointer<T, Deleter>& x,
                const external_pointer<T, Deleter>& y) {
  // Fast path: direct SEXP comparison
  SEXP x_sexp = static_cast<SEXP>(x);
  SEXP y_sexp = static_cast<SEXP>(y);
#if CPP4R_HAS_CXX20
  return x_sexp == y_sexp;
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator!=(const external_pointer<T, Deleter>& x,
                                const external_pointer<T, Deleter>& y) {
  return !(x == y);
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator<(const external_pointer<T, Deleter>& x,
                               const external_pointer<T, Deleter>& y) {
  SEXP x_sexp = static_cast<SEXP>(x);
  SEXP y_sexp = static_cast<SEXP>(y);
  return x_sexp < y_sexp;
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator<=(const external_pointer<T, Deleter>& x,
                                const external_pointer<T, Deleter>& y) {
  return !(y < x);
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator>(const external_pointer<T, Deleter>& x,
                               const external_pointer<T, Deleter>& y) {
  return y < x;
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator>=(const external_pointer<T, Deleter>& x,
                                const external_pointer<T, Deleter>& y) {
  return !(x < y);
}
#elif CPP4R_HAS_CXX17
  return x_sexp == y_sexp;
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator!=(const external_pointer<T, Deleter>& x,
                                const external_pointer<T, Deleter>& y) {
  return !(x == y);
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator<(const external_pointer<T, Deleter>& x,
                               const external_pointer<T, Deleter>& y) {
  SEXP x_sexp = static_cast<SEXP>(x);
  SEXP y_sexp = static_cast<SEXP>(y);
  return x_sexp < y_sexp;
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator<=(const external_pointer<T, Deleter>& x,
                                const external_pointer<T, Deleter>& y) {
  return !(y < x);
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator>(const external_pointer<T, Deleter>& x,
                               const external_pointer<T, Deleter>& y) {
  return y < x;
}

template <class T, void Deleter(T*)>
CPP4R_NODISCARD bool operator>=(const external_pointer<T, Deleter>& x,
                                const external_pointer<T, Deleter>& y) {
  return !(x < y);
}
#else
  return x_sexp == y_sexp;
}

template <class T, void Deleter(T*)>
bool operator!=(const external_pointer<T, Deleter>& x,
                const external_pointer<T, Deleter>& y) {
  return !(x == y);
}

template <class T, void Deleter(T*)>
bool operator<(const external_pointer<T, Deleter>& x,
               const external_pointer<T, Deleter>& y) {
  SEXP x_sexp = static_cast<SEXP>(x);
  SEXP y_sexp = static_cast<SEXP>(y);
  return x_sexp < y_sexp;
}

template <class T, void Deleter(T*)>
bool operator<=(const external_pointer<T, Deleter>& x,
                const external_pointer<T, Deleter>& y) {
  return !(y < x);
}

template <class T, void Deleter(T*)>
bool operator>(const external_pointer<T, Deleter>& x,
               const external_pointer<T, Deleter>& y) {
  return y < x;
}

template <class T, void Deleter(T*)>
bool operator>=(const external_pointer<T, Deleter>& x,
                const external_pointer<T, Deleter>& y) {
  return !(x < y);
}
#endif

}  // namespace cpp4r
