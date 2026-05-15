with Rcpp we do

```
class RcppFunctionCallback : public Callback {
public:
  RcppFunctionCallback(Timestamp when, const Rcpp::Function& func);

  void invoke() const {
    func();
  }

  Rcpp::RObject rRepresentation() const;

private:
  Rcpp::Function func;
};

... etc ...

// Add a function to the registry, to be executed at `secs` seconds in
// the future (i.e. relative to the current time).
uint64_t add(const Rcpp::Function& func, double secs);

... etc ...

ThreadArgs(
    const Rcpp::Function& func,
    int num_fds,
    struct pollfd *fds,
    double timeout,
    int loop,
    CallbackRegistryTable& table
) : ThreadArgs(num_fds, fds, timeout, loop, table) {
    callback = std::unique_ptr<Rcpp::Function>(new Rcpp::Function(func));
}
```

with cpp11 (and the current cpp4r) we use sexp for the same

```
class cpp11FunctionCallback : public Callback {
public:
  cpp11FunctionCallback(Timestamp when, SEXP func);

  void invoke() const { cpp11::function{static_cast<SEXP>(func)}(); }

  cpp11::sexp rRepresentation() const;

private:
  cpp11::sexp func;
};

... etc ...

// Add a function to the registry, to be executed at `secs` seconds in
// the future (i.e. relative to the current time).
uint64_t add(SEXP func, double secs);

... etc ...

ThreadArgs(SEXP func, int num_fds, struct pollfd *fds, double timeout,
            int loop, CallbackRegistryTable &table)
        : ThreadArgs(num_fds, fds, timeout, loop, table) {
    callback = cpp11::sexp(func);
}
```

perhaps expand function.hpp so that cpp4r has a more "flexible" function-type
instead of doing all with sexp/SEXP (tho Rapi uses SEXP internally)
