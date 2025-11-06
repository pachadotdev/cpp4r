# Objective Benchmark Suite for cpp4r

Classic canonical tests inspired by the well-established [R-Benchmark-25](https://mac.r-project.org/benchmarks/) suite. Tests core matrix operations and programming patterns.

Run this  with:

```bash
make bench
```

## Benchmark goals

1. Quantify overhead: Timing overhead when calling R functions from C++
2. Compare API styles: Direct R C API (cpp4r/cpp11) vs Rcpp's wrapper classes
3. Compare Memory management: PROTECT/UNPROTECT vs automatic garbage collection

=> The benchmark will reveal which approach is more efficient (if any) when making R and C++ interact. 

## Benchmark Tests

### I. Matrix Calculation (5 tests)

1. Matrix Manipulation (`bench_matrix_manip`)
   - Creates 2500x2500 matrix from vector
   - Transposes, reshapes, and transposes again
   - Tests: Matrix creation, transpose, reshape operations

2. Matrix Power (`bench_matrix_power`)
   - Raises 2400x2400 matrix elements to power 1000
   - Tests: Element-wise operations on large matrices

3. Vector Sorting (`bench_sort`)
   - Sorts 7,000,000 random values
   - Tests: Large vector operations and sorting

4. Cross-Product (`bench_crossprod`)
   - Computes A'A for 2800x2800 matrix
   - Tests: Matrix multiplication operations

5. Linear Regression (`bench_linear_regression`)
   - Solves linear system for 2000x2000 matrix
   - Tests: X'X and X'y computations

### II. Matrix Functions (5 tests)

These tests primarily call R functions from C++ to test interface overhead:

1. FFT (`bench_fft`)
   - FFT over 2,400,000 random values
   - Calls R's `fft()` function from C++

2. Eigenvalues (`bench_eigenvalues`)
   - Eigenvalues of 640x640 random matrix
   - Calls R's `eigen()` function from C++

3. Determinant (`bench_determinant`)
   - Determinant of 2500x2500 random matrix
   - Calls R's `det()` function from C++

4. Cholesky Decomposition (`bench_cholesky`)
   - Cholesky decomposition of 3000x3000 matrix
   - Calls R's `chol()` function from C++

5. Matrix Inverse (`bench_inverse`)
   - Inverse of 1600x1600 random matrix
   - Calls R's `solve()` function from C++

### III. Programming (5 tests)

1. Fibonacci (Vectorized) (`bench_fibonacci_vector`)
   - 3,500,000 Fibonacci numbers via Binet's formula
   - Tests: Vectorized mathematical operations

2. Hilbert Matrix (`bench_hilbert_matrix`)
   - Creates 3000x3000 Hilbert matrix
   - Tests: Nested loops and matrix creation

3. GCD (Vectorized) (`bench_gcd_vector`)
   - GCD for 400,000 pairs of integers
   - Tests: Integer operations and algorithms

4. Toeplitz Matrix (`bench_toeplitz_matrix`)
   - Creates 500x500 Toeplitz matrix
   - Tests: Nested loop performance

5. Escoufier's Method (`bench_escoufier`)
   - Full RV coefficient computation on 45x45 matrix
   - Tests: Calling R's `cor()` function AND C++ matrix operations
   - Combines R function calls with intensive C++ loops for matrix subsetting and trace computations

## Implementation Details

cpp4r Implementation
- Uses `cpp4r.hpp` header
- Direct R C API calls with `Rf_install()`, `Rf_lang*()`, `Rf_eval()`
- Explicit PROTECT/UNPROTECT for memory management
- Uses `writable::` variants for creating new objects

cpp11 Implementation
- Uses `cpp11.hpp` header
- Same R C API approach as cpp4r for calling R functions

Rcpp Implementation
- Uses `Rcpp.h` header
- More idiomatic Rcpp approach with Environment and Function objects
- Cleaner syntax for calling R functions
- Uses Rcpp's automatic garbage collection

## Key Design Decisions

1. **Calling R functions from C++**: For matrix functions (FFT, eigenvalues, etc.), all implementations call R's optimized functions rather than reimplementing them in C++. This tests the interface overhead.

2. **Identical algorithms**: All three implementations use the same algorithmic approach to ensure fair comparison.

3. **Escoufier's Method**: Implements the full algorithm from R-benchmark-25, which:
   - Calls R's `cor()` function repeatedly (tests R function call overhead)
   - Performs extensive C++ matrix operations (column binding, subsetting)
   - Computes matrix traces through C++ loops
   - This provides a comprehensive test of both R function call efficiency and C++ matrix manipulation performance
