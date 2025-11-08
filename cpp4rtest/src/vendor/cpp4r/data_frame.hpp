#pragma once

#include "cpp4r/cpp_version.hpp"  // Must be first for version detection

#include <cstdlib>           // for abs
#include <initializer_list>  // for initializer_list
#include <string>            // for string, basic_string
#include <utility>           // for move

#if CPP4R_HAS_CXX17
#include <string_view>  // for std::string_view (C++17)
#endif

#include "R_ext/Arith.h"              // for NA_INTEGER
#include "cpp4r/R.hpp"                // for Rf_xlength, SEXP, SEXPREC, INTEGER
#include "cpp4r/attribute_proxy.hpp"  // for attribute_proxy
#include "cpp4r/list.hpp"             // for list, r_vector<>::r_vector, r_v...
#include "cpp4r/r_vector.hpp"         // for r_vector

namespace cpp4r {

class named_arg;
namespace writable {
class data_frame;
}  // namespace writable

class data_frame : public list {
  using list::list;

  friend class writable::data_frame;

  /* we cannot use Rf_getAttrib because it has a special case for c(NA, -n) and creates
   * the full vector */
  static SEXP get_attrib0(SEXP x, SEXP sym) noexcept {
    for (SEXP attr = ATTRIB(x); attr != R_NilValue; attr = CDR(attr)) {
#if CPP4R_HAS_CXX20
      if (TAG(attr) == sym) {
        return CAR(attr);
      }
#else
      if (TAG(attr) == sym) {
        return CAR(attr);
      }
#endif
    }

    return R_NilValue;
  }

  static R_xlen_t calc_nrow(SEXP x) noexcept {
    auto nms = get_attrib0(x, R_RowNamesSymbol);
    bool has_short_rownames =
        (Rf_isInteger(nms) && Rf_xlength(nms) == 2 && INTEGER(nms)[0] == NA_INTEGER);
#if CPP4R_HAS_CXX20
    if (CPP4R_LIKELY(has_short_rownames)) {
      return static_cast<R_xlen_t>(abs(INTEGER(nms)[1]));
    }

    if (CPP4R_UNLIKELY(!Rf_isNull(nms))) {
      return Rf_xlength(nms);
    }

    if (CPP4R_UNLIKELY(Rf_xlength(x) == 0)) {
      return 0;
    }
#else
    if (__builtin_expect(has_short_rownames, 1)) {
      return static_cast<R_xlen_t>(abs(INTEGER(nms)[1]));
    }

    if (__builtin_expect(!Rf_isNull(nms), 0)) {
      return Rf_xlength(nms);
    }

    if (__builtin_expect(Rf_xlength(x) == 0, 0)) {
      return 0;
    }
#endif

    return Rf_xlength(VECTOR_ELT(x, 0));
  }

 public:
  /* Adapted from
   * https://github.com/wch/r-source/blob/f2a0dfab3e26fb42b8b296fcba40cbdbdbec767d/src/main/attrib.c#L198-L207
   */
#if CPP4R_HAS_CXX17
  CPP4R_NODISCARD R_xlen_t nrow() const noexcept { return calc_nrow(*this); }
  CPP4R_NODISCARD R_xlen_t ncol() const noexcept { return size(); }
#else
  R_xlen_t nrow() const noexcept { return calc_nrow(*this); }
  R_xlen_t ncol() const noexcept { return size(); }
#endif
};

namespace writable {
class data_frame : public cpp4r::data_frame {
 private:
#if CPP4R_HAS_CXX17
  // C++17: Use move semantics more efficiently
  writable::list set_data_frame_attributes(writable::list&& x) {
    return set_data_frame_attributes(std::move(x), calc_nrow(x));
  }

  writable::list set_data_frame_attributes(writable::list&& x, R_xlen_t nrow) {
    x.attr(R_RowNamesSymbol) = {NA_INTEGER, -static_cast<int>(nrow)};
    x.attr(R_ClassSymbol) = "data.frame";
    return std::move(x);
  }
#else
  // C++11/14: Standard move semantics
  writable::list set_data_frame_attributes(writable::list&& x) {
    return set_data_frame_attributes(std::move(x), calc_nrow(x));
  }

  writable::list set_data_frame_attributes(writable::list&& x, R_xlen_t nrow) {
    x.attr(R_RowNamesSymbol) = {NA_INTEGER, -static_cast<int>(nrow)};
    x.attr(R_ClassSymbol) = "data.frame";
    return std::move(x);
  }
#endif

 public:
  data_frame(const SEXP data) : cpp4r::data_frame(set_data_frame_attributes(data)) {}
  data_frame(const SEXP data, bool is_altrep)
      : cpp4r::data_frame(set_data_frame_attributes(data), is_altrep) {}
  data_frame(const SEXP data, bool is_altrep, R_xlen_t nrow)
      : cpp4r::data_frame(set_data_frame_attributes(data, nrow), is_altrep) {}
  data_frame(std::initializer_list<list> il)
      : cpp4r::data_frame(set_data_frame_attributes(writable::list(il))) {}
  data_frame(std::initializer_list<named_arg> il)
      : cpp4r::data_frame(set_data_frame_attributes(writable::list(il))) {}

  using cpp4r::data_frame::ncol;
  using cpp4r::data_frame::nrow;

#if CPP4R_HAS_CXX17
  // C++17: Return type can use [[nodiscard]]
  CPP4R_NODISCARD attribute_proxy<data_frame> attr(const char* name) const noexcept {
    return {*this, name};
  }

  CPP4R_NODISCARD attribute_proxy<data_frame> attr(
      const std::string& name) const noexcept {
    return {*this, name.c_str()};
  }

  // C++17: Add string_view overload for zero-copy
  CPP4R_NODISCARD attribute_proxy<data_frame> attr(std::string_view name) const noexcept {
    return {*this, name};
  }

  CPP4R_NODISCARD attribute_proxy<data_frame> attr(SEXP name) const noexcept {
    return {*this, name};
  }

  CPP4R_NODISCARD attribute_proxy<data_frame> names() const noexcept {
    return {*this, R_NamesSymbol};
  }
#else
  // C++11/14: No nodiscard or string_view
  attribute_proxy<data_frame> attr(const char* name) const noexcept {
    return {*this, name};
  }

  attribute_proxy<data_frame> attr(const std::string& name) const noexcept {
    return {*this, name.c_str()};
  }

  attribute_proxy<data_frame> attr(SEXP name) const noexcept { return {*this, name}; }

  attribute_proxy<data_frame> names() const noexcept { return {*this, R_NamesSymbol}; }
#endif
};

}  // namespace writable

}  // namespace cpp4r
