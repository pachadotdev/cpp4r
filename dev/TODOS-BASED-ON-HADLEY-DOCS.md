1. environment cannot create new envs (R_NewEnv)
environments.md documents R_NewEnv(enclos, hash, size) (R ≥ 4.1.0). The current environment.hpp only wraps existing SEXP environments — there's no factory constructor. A static environment::new_env() or constructor taking (enclos, hash, size) would fill this.

2. No protected evaluation wrapper
error-eval.md documents R_tryEvalSilent and R_ToplevelExec. The function::operator() in function.hpp:22-30 uses bare Rf_eval into R_GlobalEnv. Any R error propagates as a longjmp; wrapping with R_tryEvalSilent would let callers get NULL + error flag instead of crashing through C++ stack frames.

3. No weak reference support
misc.md documents R_MakeWeakRef / WEAKREFSXP. Rcpp has no equivalent. This would let cpp4r users build GC-safe caches (e.g. memoizing expensive computations keyed on R objects without preventing collection).

4. No first-class pairlist / ... traversal
pairlists.md shows the CAR/CDR loop idiom for LISTSXP. Package authors who want to handle ... arguments from .Call currently have to drop to raw SEXP. A thin pairlist range-type wrapping CAR/CDR/TAG would cover this.