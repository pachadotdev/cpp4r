cpp_compiler <- "clang"
cpp_std <- "CXXNN"

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

bench_all <- function(dat, iterations = 100L) {
    results <- list()

    results$prealloc_weighted_sum <- light_bench_results(bench::mark(
        cpp11benchmark::bench_prealloc_weighted_sum(dat$x, dat$y, dat$w),
        cpp4rbenchmark::bench_prealloc_weighted_sum(dat$x, dat$y, dat$w),
        Rcppbenchmark::bench_prealloc_weighted_sum(dat$x, dat$y, dat$w),
        iterations = iterations
    ))

    results$matrix_multiply <- light_bench_results(bench::mark(
        cpp11benchmark::bench_matrix_multiply(dat$A, dat$B),
        cpp4rbenchmark::bench_matrix_multiply(dat$A, dat$B),
        Rcppbenchmark::bench_matrix_multiply(dat$A, dat$B),
        iterations = iterations
    ))

    results$rolling_mean <- light_bench_results(bench::mark(
        cpp11benchmark::bench_rolling_mean(dat$x, 50L),
        cpp4rbenchmark::bench_rolling_mean(dat$x, 50L),
        Rcppbenchmark::bench_rolling_mean(dat$x, 50L),
        iterations = iterations
    ))

    results$dataframe_summary <- light_bench_results(bench::mark(
        cpp11benchmark::bench_dataframe_summary(dat$df),
        cpp4rbenchmark::bench_dataframe_summary(dat$df),
        Rcppbenchmark::bench_dataframe_summary(dat$df),
        iterations = iterations
    ))

    results$string_pattern_count <- light_bench_results(bench::mark(
        cpp11benchmark::bench_string_pattern_count(dat$strs, "abc"),
        cpp4rbenchmark::bench_string_pattern_count(dat$strs, "abc"),
        Rcppbenchmark::bench_string_pattern_count(dat$strs, "abc"),
        iterations = iterations
    ))

    results$grouped_mean <- light_bench_results(bench::mark(
        cpp11benchmark::bench_grouped_mean(dat$x, dat$groups),
        cpp4rbenchmark::bench_grouped_mean(dat$x, dat$groups),
        Rcppbenchmark::bench_grouped_mean(dat$x, dat$groups),
        iterations = iterations
    ))

    results$fibonacci <- light_bench_results(bench::mark(
        cpp11benchmark::bench_fibonacci(30L),
        cpp4rbenchmark::bench_fibonacci(30L),
        Rcppbenchmark::bench_fibonacci(30L),
        iterations = iterations
    ))

    results$mean_na_rm <- light_bench_results(bench::mark(
        cpp11benchmark::bench_mean_na_rm(dat$x),
        cpp4rbenchmark::bench_mean_na_rm(dat$x),
        Rcppbenchmark::bench_mean_na_rm(dat$x),
        iterations = iterations
    ))

    results$outer_product <- light_bench_results(bench::mark(
        cpp11benchmark::bench_outer_product(dat$x, dat$y),
        cpp4rbenchmark::bench_outer_product(dat$x, dat$y),
        Rcppbenchmark::bench_outer_product(dat$x, dat$y),
        iterations = iterations
    ))

    results$pairwise_distances <- light_bench_results(bench::mark(
        cpp11benchmark::bench_pairwise_distances(dat$pairwise_x),
        cpp4rbenchmark::bench_pairwise_distances(dat$pairwise_x),
        Rcppbenchmark::bench_pairwise_distances(dat$pairwise_x),
        iterations = iterations
    ))

    results$cumulative_ops <- light_bench_results(bench::mark(
        cpp11benchmark::bench_cumulative_ops(dat$x),
        cpp4rbenchmark::bench_cumulative_ops(dat$x),
        Rcppbenchmark::bench_cumulative_ops(dat$x),
        iterations = iterations
    ))

    results$bootstrap_mean <- light_bench_results(bench::mark(
        { set.seed(123L); cpp11benchmark::bench_bootstrap_mean(dat$x, 1000L) },
        { set.seed(123L); cpp4rbenchmark::bench_bootstrap_mean(dat$x, 1000L) },
        { set.seed(123L); Rcppbenchmark::bench_bootstrap_mean(dat$x, 1000L) },
        iterations = iterations
    ))

    results
}

sizes <- c("small", "medium", "large")

bench_results <- list()

for (sz in sizes) {
    path <- file.path("./", paste0("bench_data_", sz, ".rds"))
    if (!file.exists(path)) {
        message("Skipping missing dataset: ", path)
        next
    }
    message("Running benchmarks for: ", sz)
    dat <- readRDS(path)
    bench_results[[sz]] <- bench_all(dat, iterations = 100L)

    # assign(paste0("bench_data_", sz), dat, envir = .GlobalEnv)

    saveRDS(bench_results[[sz]], file = file.path("./", paste0("bench_results_", sz, "_",
        cpp_compiler, "_", cpp_std, ".rds")))
    gc()
}
