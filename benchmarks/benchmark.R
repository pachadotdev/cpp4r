cpp_std <- "CXX11"

library(bench)

bench_all <- function(dat, iterations = 10L) {
    results <- list()

    results$prealloc_weighted_sum <- bench::mark(
        cpp11benchmark::bench_prealloc_weighted_sum(dat$x, dat$y, dat$w),
        cpp4rbenchmark::bench_prealloc_weighted_sum(dat$x, dat$y, dat$w),
        Rcppbenchmark::bench_prealloc_weighted_sum(dat$x, dat$y, dat$w),
        iterations = iterations
    )

    results$matrix_multiply <- bench::mark(
        cpp11benchmark::bench_matrix_multiply(dat$A, dat$B),
        cpp4rbenchmark::bench_matrix_multiply(dat$A, dat$B),
        Rcppbenchmark::bench_matrix_multiply(dat$A, dat$B),
        iterations = iterations
    )

    results$rolling_mean <- bench::mark(
        cpp11benchmark::bench_rolling_mean(dat$x, 50L),
        cpp4rbenchmark::bench_rolling_mean(dat$x, 50L),
        Rcppbenchmark::bench_rolling_mean(dat$x, 50L),
        iterations = iterations
    )

    results$dataframe_summary <- bench::mark(
        cpp11benchmark::bench_dataframe_summary(dat$df),
        cpp4rbenchmark::bench_dataframe_summary(dat$df),
        Rcppbenchmark::bench_dataframe_summary(dat$df),
        iterations = iterations
    )

    results$string_pattern_count <- bench::mark(
        cpp11benchmark::bench_string_pattern_count(dat$strs, "abc"),
        cpp4rbenchmark::bench_string_pattern_count(dat$strs, "abc"),
        Rcppbenchmark::bench_string_pattern_count(dat$strs, "abc"),
        iterations = iterations
    )

    results$grouped_mean <- bench::mark(
        cpp11benchmark::bench_grouped_mean(dat$x, dat$groups),
        cpp4rbenchmark::bench_grouped_mean(dat$x, dat$groups),
        Rcppbenchmark::bench_grouped_mean(dat$x, dat$groups),
        iterations = iterations
    )

    results$fibonacci <- bench::mark(
        cpp11benchmark::bench_fibonacci(30L),
        cpp4rbenchmark::bench_fibonacci(30L),
        Rcppbenchmark::bench_fibonacci(30L),
        iterations = iterations
    )

    results$mean_na_rm <- bench::mark(
        cpp11benchmark::bench_mean_na_rm(dat$x),
        cpp4rbenchmark::bench_mean_na_rm(dat$x),
        Rcppbenchmark::bench_mean_na_rm(dat$x),
        iterations = iterations
    )

    results$outer_product <- bench::mark(
        cpp11benchmark::bench_outer_product(dat$x, dat$y),
        cpp4rbenchmark::bench_outer_product(dat$x, dat$y),
        Rcppbenchmark::bench_outer_product(dat$x, dat$y),
        iterations = iterations
    )

    results$pairwise_distances <- bench::mark(
        cpp11benchmark::bench_pairwise_distances(dat$pairwise_x),
        cpp4rbenchmark::bench_pairwise_distances(dat$pairwise_x),
        Rcppbenchmark::bench_pairwise_distances(dat$pairwise_x),
        iterations = iterations
    )

    results$cumulative_ops <- bench::mark(
        cpp11benchmark::bench_cumulative_ops(dat$x),
        cpp4rbenchmark::bench_cumulative_ops(dat$x),
        Rcppbenchmark::bench_cumulative_ops(dat$x),
        iterations = iterations
    )

    results$bootstrap_mean <- bench::mark(
        cpp11benchmark::bench_bootstrap_mean(dat$x, 1000L),
        cpp4rbenchmark::bench_bootstrap_mean(dat$x, 1000L),
        Rcppbenchmark::bench_bootstrap_mean(dat$x, 1000L),
        iterations = iterations
    )

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
    bench_results[[sz]] <- bench_all(dat, iterations = 10L)

    # assign(paste0("bench_data_", sz), dat, envir = .GlobalEnv)

    saveRDS(bench_results[[sz]], file = file.path("./", paste0("bench_results_", sz, "_", cpp_std, ".rds")))
    gc()
}
