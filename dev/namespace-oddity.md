Capture a function's namespace when parsing #12
===============================================

When parsing attributes, decor currently isn't away if the function is in a namespace.

```r
library(decor)
library(withr)

withr::with_tempfile("f", {
  writeLines("
    namespace example {
        [[cpp11::register]]
        int fun(int x = 1) {
            return x + 1; 
        }
    }", 
    f
  )
  decor::cpp_decorations(files = f, is_attribute = TRUE)
})
```

Which makes it impossible to properly register these functions using cpp11. That would require something like this:

```cpp
// file.cpp
    int example::fun();
extern "C" SEXP _example_fun() {
  BEGIN_CPP11
    return cpp11::as_sexp(example::fun());
  END_CPP11
}
```

But without that namespace information, it's not possible to use this pattern.

This is somewhat related to https://github.com/r-lib/cpp11/issues/262 and should make things a little simpler on authors.

Thanks!


Register function with namespace #262
=======================================

When registering a function in a namespace such as

```cpp
[[cpp11::register]] int foo::bar(int a) {
  return a;
}
```

decor recognizes the function name as `foo::bar` and all would be fine (as far as I can tell), if the generated `extern "C"` wrapper 

https://github.com/r-lib/cpp11/blob/c22dc9b611fe7d56ddfc57add2e01b1424869c0f/R/register.R#L171-L176

would not blindly forward the `::`. Some further minor adjustments are required for the generation of the R-side functions. For the R-side function name, we could for example strip away anything that comes before the last scope resolution operator.

I started putting something together under https://github.com/r-lib/cpp11/compare/main...nbenn:main and I'm happy to clean this up a bit, add some tests and submit a PR.
