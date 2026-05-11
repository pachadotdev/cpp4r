#pragma once

#include "cpp4r/R.hpp"        // for SEXP, R_NilValue, WEAKREFSXP
#include "cpp4r/protect.hpp"  // for safe, detail::store
#include "cpp4r/r_bool.hpp"   // for r_bool
#include "cpp4r/sexp.hpp"     // for sexp

namespace cpp4r {

// A weak reference to an R object.
//
// A weak_ref holds a key and a value. When the key becomes unreachable by the
// R garbage collector, the value is set to R_NilValue and the optional
// finalizer (C callback) is run.
//
// IMPORTANT: The key must be a *reference object* — i.e. an environment
// (ENVSXP), external pointer (EXTPTRSXP), weak reference (WEAKREFSXP), or
// R_NilValue (no key). Passing a plain vector (INTSXP, REALSXP, etc.) will
// trigger an R error. This is a constraint of R's C API (R_MakeWeakRef).
//
// This is the basis for GC-safe caches: store computed results keyed on the
// input R object. If the input is collected, the cache entry is automatically
// cleared — no dangling pointers, no memory leak.
//
// Usage:
//
//   // Cache a result keyed on an environment, invalidated when env is GC'd.
//   cpp4r::weak_ref cache(env_sexp, result);
//
//   SEXP cached_val = cache.value();  // R_NilValue if key was collected
//   bool alive      = cache.alive();
//
// Finalizer usage (runs just before the key is collected):
//
//   void on_collect(SEXP weakref) {
//     // weakref is the WEAKREFSXP itself
//   }
//
//   cpp4r::weak_ref cache(env_sexp, result, on_collect, /*onexit=*/false);

class weak_ref {
 public:
  // Create a weak reference with no finalizer.
  // key:    the object whose liveness controls this reference.
  //         Must be ENVSXP, EXTPTRSXP, WEAKREFSXP, or R_NilValue.
  // value:  the associated value (set to R_NilValue when key is collected).
  // onexit: if true, run the finalizer on R session exit even if key is alive.
  weak_ref(SEXP key, SEXP value, Rboolean onexit = FALSE)
      : data_(make(validate_key(key), value, R_NilValue, onexit)) {}

  // Create a weak reference with a C finalizer.
  // The finalizer receives the WEAKREFSXP and runs when the key is collected.
  using finalizer_t = R_CFinalizer_t;  // void (*)(SEXP weakref)

  weak_ref(SEXP key, SEXP value, finalizer_t fin, bool onexit = false)
      : data_(make_with_c_finalizer(validate_key(key), value, fin,
                                    onexit ? TRUE : FALSE)) {}

  // The key object. Returns R_NilValue if collected.
  SEXP key() const noexcept { return R_WeakRefKey(data_); }

  // The value object. Returns R_NilValue once the key has been collected.
  SEXP value() const noexcept { return R_WeakRefValue(data_); }

  // True if the key has not yet been collected.
  bool alive() const noexcept { return key() != R_NilValue; }

  // Run the finalizer immediately (regardless of key liveness).
  void run_finalizer() noexcept { R_RunWeakRefFinalizer(data_); }

  operator SEXP() const noexcept { return data_; }

 private:
  sexp data_;

  static SEXP validate_key(SEXP key) {
    SEXPTYPE t = TYPEOF(key);
    if (t != NILSXP && t != ENVSXP && t != EXTPTRSXP && t != WEAKREFSXP) {
      cpp4r::stop(
          "weak_ref key must be an environment, external pointer, or weak "
          "reference (got %s)",
          Rf_type2char(t));
    }
    return key;
  }

  static sexp make(SEXP key, SEXP value, SEXP fin, Rboolean onexit) {
    return safe[R_MakeWeakRef](key, value, fin, onexit);
  }

  static sexp make_with_c_finalizer(SEXP key, SEXP value, finalizer_t fin,
                                    Rboolean onexit) {
    return safe[R_MakeWeakRefC](key, value, fin, onexit);
  }
};

}  // namespace cpp4r
