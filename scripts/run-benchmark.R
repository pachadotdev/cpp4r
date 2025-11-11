cpp_compiler <- "gcc"
cpp_std <- "CXX23"

library(bench)
library(cpp11benchmark)
library(cpp4rbenchmark)
library(Rcppbenchmark)

try(dir.create("./results/"), silent = TRUE)

message(
    "\n==============================================================================="
)
message("\nRunning benchmarks")
message(
    "\nSee https://arma.sourceforge.net/speed.html for details on the benchmark tests."
)
message(
    "\n==============================================================================="
)

n <- 500L
iterations <- 100L

set.seed(42)
A <- matrix(rnorm(n * n), nrow = n, ncol = n)
B <- matrix(rnorm(n * n), nrow = n, ncol = n)
C <- matrix(rnorm(n * n), nrow = n, ncol = n)
D <- matrix(rnorm(n * n), nrow = n, ncol = n)

# Add two matrices
# Z = A+B

add_two <- function(A, B) {
    Z <- A + B
    Z
}

# Add four matrices
# Z = A+B+C+D

add_four <- function(A, B, C, D) {
    Z <- A + B + C + D
    Z
}

# Multiply four matrices
# Z = A*B*C*D
# Here matrix A has more elements than B, which in turn has more elements than C, and so on.
# In this case it is more efficient to multiply the matrices "backwards", which is automatically done by Armadillo.

multiply_four <- function(A, B, C, D) {
    n = ncol(A)
    n5 = n %/% 5L
    n10 = n %/% 10L
    n15 = n %/% 15L
    n20 = n %/% 20L
    Z <- A[seq_len(n5), seq_len(n5)] %*%
        B[seq_len(n5), seq_len(n10)] %*%
        C[seq_len(n10), seq_len(n15)] %*%
        D[seq_len(n15), seq_len(n20)]
    Z
}

# Submatrix manipulation
# B.row(size-1) = A.row(0)
# Copy first row of matrix A into last row of matrix B

submatrix_manipulation <- function(A, B) {
    B[n, ] <- A[1L, ]
    B
}

# Multi-operation expression
# Multiply the transpose of a column vector by the inverse of a diagonal matrix, then multiply by a column vector.

multi_operation <- function(A, B, C) {
    Z <- t(A[, 1L]) %*% solve(diag(diag(B))) %*% C[, 1L]
    as.numeric(Z)
}

light_bench_results <- function(b) {
    b <- b[, c("expression", "mem_alloc", "time")]

    res <- tibble::tibble(
        backend = as.character(b$expression),
        mem_alloc = as.numeric(b$mem_alloc) / 1e6,
        time = lapply(b$time, as.numeric)
    )

    res
}

bench_cpp <- function(d, iterations = 1L) {
    results <- list()

    message("\n Test 1: Add Two Matrices\n")

    # x = add_two(A, B)
    # y = add_two_cpp4r(A, B)
    # z = add_two_rcpp(A, B)
    # p = add_two_cpp11(A, B)
    # all.equal(x, y)
    # all.equal(x, z)
    # all.equal(x, p)

    results$add_two <- light_bench_results(
        bench::mark(
            add_two(A, B),
            add_two_cpp11(A, B),
            add_two_cpp4r(A, B),
            add_two_rcpp(A, B),
            iterations = iterations
        )
    )

    message("\n Test 2: Add Four Matrices\n")

    results$add_four <- light_bench_results(
        bench::mark(
            add_four(A, B, C, D),
            add_four_cpp11(A, B, C, D),
            add_four_cpp4r(A, B, C, D),
            add_four_rcpp(A, B, C, D),
            iterations = iterations
        )
    )

    message("\n Test 3: Multiply Four Matrices\n")

    results$multiply_four <- light_bench_results(
        bench::mark(
            multiply_four(A, B, C, D),
            multiply_four_cpp11(A, B, C, D),
            multiply_four_cpp4r(A, B, C, D),
            multiply_four_rcpp(A, B, C, D),
            iterations = iterations
        )
    )

    message("\n Test 4: Submatrix Manipulation\n")

    results$submatrix_manipulation <- light_bench_results(
        bench::mark(
            submatrix_manipulation(A, B),
            submatrix_manipulation_cpp11(A, B),
            submatrix_manipulation_cpp4r(A, B),
            submatrix_manipulation_rcpp(A, B),
            iterations = iterations
        )
    )

    message("\n Test 5: Multi-Operation Expression\n")

    results$multi_operation <- light_bench_results(
        bench::mark(
            multi_operation(A, B, C),
            multi_operation_cpp11(A, B, C),
            multi_operation_cpp4r(A, B, C),
            multi_operation_rcpp(A, B, C),
            iterations = iterations
        )
    )

    results
}

fout <- file.path(
    "./extended-tests-results/",
    paste0("bench_results_", cpp_compiler, "_", cpp_std, ".rds")
)

if (!file.exists(fout)) {
    bench_results <- bench_cpp(d, iterations = iterations)

    saveRDS(
        bench_results,
        file = fout,
        compress = "xz"
    )

    message(sprintf(
        "Results saved to bench_results_%s_%s.rds\n",
        cpp_compiler,
        cpp_std
    ))

    gc()
}
