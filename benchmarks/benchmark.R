cpp_compiler <- "gcc"
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
    
    results$matrix_manip <- light_bench_results(bench::mark(
        cpp11benchmark::bench_matrix_manip(dat$matrix_manip_vec, dat$matrix_manip_nrow, dat$matrix_manip_ncol),
        cpp4rbenchmark::bench_matrix_manip(dat$matrix_manip_vec, dat$matrix_manip_nrow, dat$matrix_manip_ncol),
        Rcppbenchmark::bench_matrix_manip(dat$matrix_manip_vec, dat$matrix_manip_nrow, dat$matrix_manip_ncol),
        iterations = iterations
    ))

    results$matrix_power <- light_bench_results(bench::mark(
        cpp11benchmark::bench_matrix_power(dat$matrix_power, dat$power_exponent),
        cpp4rbenchmark::bench_matrix_power(dat$matrix_power, dat$power_exponent),
        Rcppbenchmark::bench_matrix_power(dat$matrix_power, dat$power_exponent),
        iterations = iterations
    ))

    results$sort <- light_bench_results(bench::mark(
        cpp11benchmark::bench_sort(dat$sort_vec),
        cpp4rbenchmark::bench_sort(dat$sort_vec),
        Rcppbenchmark::bench_sort(dat$sort_vec),
        iterations = iterations
    ))

    results$crossprod <- light_bench_results(bench::mark(
        cpp11benchmark::bench_crossprod(dat$crossprod_mat),
        cpp4rbenchmark::bench_crossprod(dat$crossprod_mat),
        Rcppbenchmark::bench_crossprod(dat$crossprod_mat),
        iterations = iterations
    ))

    results$linear_regression <- light_bench_results(bench::mark(
        cpp11benchmark::bench_linear_regression(dat$linreg_X, dat$linreg_y),
        cpp4rbenchmark::bench_linear_regression(dat$linreg_X, dat$linreg_y),
        Rcppbenchmark::bench_linear_regression(dat$linreg_X, dat$linreg_y),
        iterations = iterations
    ))

    # II. Matrix Functions Tests
    cat("Running II. Matrix Functions benchmarks...\n")

    results$determinant <- light_bench_results(bench::mark(
        cpp11benchmark::bench_determinant(dat$det_mat),
        cpp4rbenchmark::bench_determinant(dat$det_mat),
        Rcppbenchmark::bench_determinant(dat$det_mat),
        iterations = iterations
    ))

    # III. Programming Tests
    cat("Running III. Programming benchmarks...\n")

    results$fibonacci_vector <- light_bench_results(bench::mark(
        cpp11benchmark::bench_fibonacci_vector(dat$fib_indices),
        cpp4rbenchmark::bench_fibonacci_vector(dat$fib_indices),
        Rcppbenchmark::bench_fibonacci_vector(dat$fib_indices),
        iterations = iterations
    ))

    results$hilbert_matrix <- light_bench_results(bench::mark(
        cpp11benchmark::bench_hilbert_matrix(dat$hilbert_n),
        cpp4rbenchmark::bench_hilbert_matrix(dat$hilbert_n),
        Rcppbenchmark::bench_hilbert_matrix(dat$hilbert_n),
        iterations = iterations
    ))

    results$gcd_vector <- light_bench_results(bench::mark(
        cpp11benchmark::bench_gcd_vector(dat$gcd_x, dat$gcd_y),
        cpp4rbenchmark::bench_gcd_vector(dat$gcd_x, dat$gcd_y),
        Rcppbenchmark::bench_gcd_vector(dat$gcd_x, dat$gcd_y),
        iterations = iterations
    ))

    results$toeplitz_matrix <- light_bench_results(bench::mark(
        cpp11benchmark::bench_toeplitz_matrix(dat$toeplitz_n),
        cpp4rbenchmark::bench_toeplitz_matrix(dat$toeplitz_n),
        Rcppbenchmark::bench_toeplitz_matrix(dat$toeplitz_n),
        iterations = iterations
    ))

    results$escoufier <- light_bench_results(bench::mark(
        cpp11benchmark::bench_escoufier(dat$escoufier_mat),
        cpp4rbenchmark::bench_escoufier(dat$escoufier_mat),
        Rcppbenchmark::bench_escoufier(dat$escoufier_mat),
        iterations = iterations
    ))

    results
}

# sizes <- c("small", "medium", "large")
sizes <- "small"

bench_results <- list()

for (sz in sizes) {
    path <- file.path("./", paste0("bench_data_", sz, ".rds"))
    if (!file.exists(path)) {
        message("Skipping missing dataset: ", path)
        next
    }
    message("Running R-benchmark-25 style benchmarks for: ", sz)
    dat <- readRDS(path)
    bench_results[[sz]] <- bench_all_rbench(dat, iterations = 100L)

    saveRDS(bench_results[[sz]], file = file.path("./", paste0("rbench_results_", sz, "_",
        cpp_compiler, "_", cpp_std, ".rds")))
    gc()
}

cat("\n=== R-Benchmark-25 Style Benchmark Complete ===\n")
cat("Results saved to rbench_results_<size>_<compiler>_<std>.rds\n")
