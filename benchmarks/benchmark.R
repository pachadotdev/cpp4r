cpp_compiler <- "clang"
cpp_std <- "CXX11"

library(bench)

light_bench_results <- function(b) {
    b <- b[, c("mem_alloc", "time")]

    res <- data.frame(
        backend = c("cpp11", "cpp4r", "Rcpp"),
        mem_alloc = as.numeric(b$mem_alloc) / 1e6
    )

    res2 <- t(vapply(
        b$time,
        function(x) {
            as.numeric(quantile(x))
        },
        numeric(5L)
    ))

    colnames(res2) <- c("min", "q25", "median", "q75", "max")

    cbind(res, res2)
}

bench_all_rbench <- function(dat, iterations = 100L) {
    results <- list()

    # I. Matrix Calculation Tests
    cat("Running I. Matrix Calculation benchmarks...\n")

    cat("  Test 1: Creation, transpose, deformation of a 2500x2500 matrix\n")
    results$matrix_manip <- light_bench_results(bench::mark(
        cpp11benchmark::bench_matrix_manip(
            dat$matrix_manip_vec,
            dat$matrix_manip_nrow,
            dat$matrix_manip_ncol
        ),
        cpp4rbenchmark::bench_matrix_manip(
            dat$matrix_manip_vec,
            dat$matrix_manip_nrow,
            dat$matrix_manip_ncol
        ),
        Rcppbenchmark::bench_matrix_manip(
            dat$matrix_manip_vec,
            dat$matrix_manip_nrow,
            dat$matrix_manip_ncol
        ),
        iterations = iterations
    ))

    cat("  Test 2: 2400x2400 normal distributed random matrix ^1000\n")
    results$matrix_power <- light_bench_results(bench::mark(
        cpp11benchmark::bench_matrix_power(
            dat$matrix_power,
            dat$power_exponent
        ),
        cpp4rbenchmark::bench_matrix_power(
            dat$matrix_power,
            dat$power_exponent
        ),
        Rcppbenchmark::bench_matrix_power(dat$matrix_power, dat$power_exponent),
        iterations = iterations
    ))

    cat("  Test 3: Sorting of 7,000,000 random values\n")
    results$sort <- light_bench_results(bench::mark(
        cpp11benchmark::bench_sort(dat$sort_vec),
        cpp4rbenchmark::bench_sort(dat$sort_vec),
        Rcppbenchmark::bench_sort(dat$sort_vec),
        iterations = iterations
    ))

    cat("  Test 4: 2800x2800 cross-product matrix (b = a' * a)\n")
    results$crossprod <- light_bench_results(bench::mark(
        cpp11benchmark::bench_crossprod(dat$crossprod_mat),
        cpp4rbenchmark::bench_crossprod(dat$crossprod_mat),
        Rcppbenchmark::bench_crossprod(dat$crossprod_mat),
        iterations = iterations
    ))

    cat("  Test 5: Linear regression over a 2000x2000 matrix\n")
    results$linear_regression <- light_bench_results(bench::mark(
        cpp11benchmark::bench_linear_regression(dat$linreg_X, dat$linreg_y),
        cpp4rbenchmark::bench_linear_regression(dat$linreg_X, dat$linreg_y),
        Rcppbenchmark::bench_linear_regression(dat$linreg_X, dat$linreg_y),
        iterations = iterations
    ))

    # II. Matrix Functions Tests
    cat("\nRunning II. Matrix Functions benchmarks...\n")

    cat("  Test 1: FFT over 2,400,000 random values\n")
    results$fft <- light_bench_results(bench::mark(
        cpp11benchmark::bench_fft(dat$fft_vec),
        cpp4rbenchmark::bench_fft(dat$fft_vec),
        Rcppbenchmark::bench_fft(dat$fft_vec),
        iterations = iterations
    ))

    cat("  Test 2: Eigenvalues of a 640x640 random matrix\n")
    results$eigenvalues <- light_bench_results(bench::mark(
        cpp11benchmark::bench_eigenvalues(dat$eigen_mat),
        cpp4rbenchmark::bench_eigenvalues(dat$eigen_mat),
        Rcppbenchmark::bench_eigenvalues(dat$eigen_mat),
        iterations = iterations
    ))

    cat("  Test 3: Determinant of a 2500x2500 random matrix\n")
    results$determinant <- light_bench_results(bench::mark(
        cpp11benchmark::bench_determinant(dat$det_mat),
        cpp4rbenchmark::bench_determinant(dat$det_mat),
        Rcppbenchmark::bench_determinant(dat$det_mat),
        iterations = iterations
    ))

    cat("  Test 4: Cholesky decomposition of a 3000x3000 matrix\n")
    results$cholesky <- light_bench_results(bench::mark(
        cpp11benchmark::bench_cholesky(dat$chol_mat),
        cpp4rbenchmark::bench_cholesky(dat$chol_mat),
        Rcppbenchmark::bench_cholesky(dat$chol_mat),
        iterations = iterations
    ))

    cat("  Test 5: Inverse of a 1600x1600 random matrix\n")
    results$inverse <- light_bench_results(bench::mark(
        cpp11benchmark::bench_inverse(dat$inv_mat),
        cpp4rbenchmark::bench_inverse(dat$inv_mat),
        Rcppbenchmark::bench_inverse(dat$inv_mat),
        iterations = iterations
    ))

    # III. Programming Tests
    cat("\nRunning III. Programming benchmarks...\n")

    cat("  Test 1: 3,500,000 Fibonacci numbers calculation (vector calc)\n")
    results$fibonacci_vector <- light_bench_results(bench::mark(
        cpp11benchmark::bench_fibonacci_vector(dat$fib_indices),
        cpp4rbenchmark::bench_fibonacci_vector(dat$fib_indices),
        Rcppbenchmark::bench_fibonacci_vector(dat$fib_indices),
        iterations = iterations
    ))

    cat("  Test 2: Creation of a 3000x3000 Hilbert matrix (matrix calc)\n")
    results$hilbert_matrix <- light_bench_results(bench::mark(
        cpp11benchmark::bench_hilbert_matrix(dat$hilbert_n),
        cpp4rbenchmark::bench_hilbert_matrix(dat$hilbert_n),
        Rcppbenchmark::bench_hilbert_matrix(dat$hilbert_n),
        iterations = iterations
    ))

    cat("  Test 3: Grand common divisors of 400,000 pairs (recursion)\n")
    results$gcd_vector <- light_bench_results(bench::mark(
        cpp11benchmark::bench_gcd_vector(dat$gcd_x, dat$gcd_y),
        cpp4rbenchmark::bench_gcd_vector(dat$gcd_x, dat$gcd_y),
        Rcppbenchmark::bench_gcd_vector(dat$gcd_x, dat$gcd_y),
        iterations = iterations
    ))

    cat("  Test 4: Creation of a 500x500 Toeplitz matrix (loops)\n")
    results$toeplitz_matrix <- light_bench_results(bench::mark(
        cpp11benchmark::bench_toeplitz_matrix(dat$toeplitz_n),
        cpp4rbenchmark::bench_toeplitz_matrix(dat$toeplitz_n),
        Rcppbenchmark::bench_toeplitz_matrix(dat$toeplitz_n),
        iterations = iterations
    ))

    cat("  Test 5: Escoufier's method on a 45x45 matrix (mixed)\n")
    results$escoufier <- light_bench_results(bench::mark(
        cpp11benchmark::bench_escoufier(dat$escoufier_mat),
        cpp4rbenchmark::bench_escoufier(dat$escoufier_mat),
        Rcppbenchmark::bench_escoufier(dat$escoufier_mat),
        iterations = iterations
    ))

    results
}

try(dir.create("./results/"), silent = TRUE)

message("Running R-benchmark-25 style benchmarks")

path <- file.path("./bench_data.rds")
dat <- readRDS(path)
bench_results <- bench_all_rbench(dat, iterations = 100L)

saveRDS(
    bench_results,
    file = file.path(
        "./results/",
        paste0("bench_results_", cpp_compiler, "_", cpp_std, ".rds")
    )
)

gc()

cat("\n=== R-Benchmark-25 Style Benchmark Complete ===\n")
cat("Results saved to rbench_results_<compiler>_<std>.rds\n")
