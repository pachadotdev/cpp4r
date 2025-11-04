# Objective Benchmark Suite for cpp4r

This benchmark suite is designed to provide neutral and reproducible performance comparisons between cpp11, cpp4r, and Rcpp. We explicitly avoid the problems of biased benchmarks by following these principles:

1. Test realistic use cases
  - Similar to code that users actually write
  - Common operations in data analysis, statistics, and numerical computing
2. Use recommended patterns for each package
  - Pre-allocate result vectors/matrices when size is known
  - No abuse of `push_back()` or other anti-patterns
  - Use direct pointer access where appropriate
  - Follow each package's documented best practices
3. Test multiple data sizes
  - Small (n = 1,000)
  - Medium (n = 10,000)
  - Large (n = 100,000)
4. Measure speed and memory
  - Median execution time
  - Memory allocation
  - Distribution of timings (not just single values)
5. Test across C++ standards
  - C++11, C++14, C++17, C++20, and C++23
  - Identify if newer standards provide benefits

## Benchmark Categories

Numerical Computing
1. Pre-allocated Weighted Sum - Vectorized operations with known size
2. Matrix Multiplication - Dense linear algebra
3. Rolling Window Mean - Sequential time series operations
4. Outer Product - Matrix generation from vectors
5. Pairwise Distance Matrix - Realistic algorithm (O(n^2))

Data Wrangling
6. Data Frame Summary - Multi-column statistics
7. Grouped Aggregation - Group-by operations
8. Cumulative Operations - Running sums, products, min/max

Statistical Computing
9. Bootstrap Resampling - Random sampling with replacement
10. NA Handling - Robust missing value handling

Other Operations
11. Fibonacci Sequence - Pure computation without R API overhead
12. String Pattern Matching - Text processing

## Running the benchmarks

```bash
# Run all 10 combinations (2 compilers × 5 standards)
make bench

# Run specific combination
make bench-specific STD=CXX17 COMPILER=clang
make bench-gcc-cxx11
make bench-clang-cxx17
```

## References

- Rewriting R code in C++: http://adv-r.hadley.nz/rcpp.html
- cpp11 documentation: https://cpp11.r-lib.org/
- R Internals: https://cran.r-project.org/doc/manuals/r-release/R-ints.html

## License

Apache License 2.0
