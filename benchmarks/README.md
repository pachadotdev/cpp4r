# Objective Benchmark Suite for cpp4r

Classic canonical tests inspired by the well-established [R-Benchmark-25](https://mac.r-project.org/benchmarks/) suite. Tests core matrix operations and programming patterns.

Run this  with:

```bash
# Run all 10 combinations (2 compilers × 5 standards)
make bench
```

## Test Categories

**I. Matrix Calculation**

1. Matrix Creation/Transpose/Reshape
2. Matrix Power (A^1000)
3. Vector Sorting (7M elements)
4. Cross-Product (A'A)
5. Linear Regression

**II. Matrix Functions**

1. Determinant (LU decomposition)

**III. Programming**

1. Fibonacci (vectorized, 3.5M values)
2. Hilbert Matrix creation
3. GCD Vector (400K pairs)
4. Toeplitz Matrix (nested loops)
5. Escoufier's Method
