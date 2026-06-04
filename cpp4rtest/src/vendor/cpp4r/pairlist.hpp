#pragma once

#include <stddef.h>  // for ptrdiff_t
#include <iterator>  // for std::forward_iterator_tag

#include "cpp4r/R.hpp"        // for R's C interface (e.g., for SEXP)
#include "cpp4r/list.hpp"     // for list (Rf_PairToVectorList result)
#include "cpp4r/protect.hpp"  // for detail::store, safe, stop
#include "cpp4r/sexp.hpp"     // for sexp

namespace cpp4r {

// Read-only forward range over a pairlist (LISTSXP, DOTSXP, or NILSXP).
//
// Pairlists are linked CAR/CDR/TAG cells used for:
//   - function formals (LISTSXP)
//   - `...` arguments  (DOTSXP, accessed via findVar(R_DotsSymbol, env))
//   - NULL / empty     (NILSXP)
//
// They are structurally distinct from R lists (VECSXP): there is no O(1)
// random access, and names are stored as TAG symbols, not a names attribute.
//
// Usage:
//
//   // Iterate over `...` forwarded into a .Call entry point
//   SEXP dots_sexp = Rf_findVar(R_DotsSymbol, env);
//   cpp4r::pairlist dots(dots_sexp);
//   for (auto node : dots) {
//     SEXP value    = node.value;            // CAR
//     const char* n = node.tag_name();       // TAG name, or nullptr if unnamed
//   }
//
//   // Convert to a named list for further processing
//   cpp4r::list lst = dots.to_list();

class pairlist {
 public:
  // A single CAR/TAG element from the pairlist.
  struct node {
    SEXP value;  // CAR — the element value (may be R_MissingArg for missing `...` args)
    SEXP tag;    // TAG — a SYMSXP naming the element, or R_NilValue if unnamed

    // Convenience: returns the tag name as a C string, or nullptr if unnamed.
    const char* tag_name() const noexcept {
      return tag != R_NilValue ? CHAR(PRINTNAME(tag)) : nullptr;
    }
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = node;
    using difference_type = ptrdiff_t;
    using pointer = const node*;
    using reference = node;

    explicit const_iterator(SEXP cons) noexcept : cons_(cons) {}

    node operator*() const noexcept { return {CAR(cons_), TAG(cons_)}; }

    const_iterator& operator++() noexcept {
      cons_ = CDR(cons_);
      return *this;
    }

    const_iterator operator++(int) noexcept {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const const_iterator& rhs) const noexcept {
      return cons_ == rhs.cons_;
    }
    bool operator!=(const const_iterator& rhs) const noexcept {
      return cons_ != rhs.cons_;
    }

   private:
    SEXP cons_;
  };

  // Construct from a LISTSXP, DOTSXP, or NILSXP.
  //
  // For `...`, Rf_findVar(R_DotsSymbol, env) returns R_MissingArg when there are
  // no arguments. This is treated as an empty pairlist.
  explicit pairlist(SEXP data)
      : data_(validate(data)), protect_(detail::store::insert(data_)) {}

  ~pairlist() { detail::store::release(protect_); }

  const_iterator begin() const noexcept { return const_iterator(data_); }
  const_iterator end() const noexcept { return const_iterator(R_NilValue); }

  R_len_t size() const noexcept { return Rf_length(data_); }
  bool empty() const noexcept { return data_ == R_NilValue; }

  // Convert to a VECSXP-backed cpp4r::list. Names are taken from TAG symbols.
  // Returns an empty list if this pairlist is empty.
  list to_list() const {
    if (data_ == R_NilValue) {
      return safe[Rf_allocVector](VECSXP, 0);
    }
    return safe[Rf_PairToVectorList](data_);
  }

  operator SEXP() const noexcept { return data_; }

 private:
  SEXP data_;
  SEXP protect_;

  static SEXP validate(SEXP x) {
    // R_MissingArg is returned by findVar(R_DotsSymbol, env) when `...` is empty.
    if (x == R_MissingArg || x == R_NilValue) {
      return R_NilValue;
    }
    SEXPTYPE t = TYPEOF(x);
    if (t != LISTSXP && t != DOTSXP) {
      cpp4r::stop("Expected a pairlist (LISTSXP or DOTSXP), not %s", Rf_type2char(t));
    }
    return x;
  }
};

}  // namespace cpp4r
