install("cpp11sampling", args = "--no-multiarch", upgrade = "never")
install("cpp4rsampling", args = "--no-multiarch", upgrade = "never")
install("Rcppsampling", args = "--no-multiarch", upgrade = "never")

library(cpp11sampling)
library(cpp4rsampling)
library(Rcppsampling)
library(bench)
library(purrr)

rejection_sizes <- seq(25000, 100000, by = 25000)

rejection_bench <- map_df(
  rejection_sizes,
  function(n_samples) {
    cat("Target samples:", n_samples, "\n")
    d <- bench::mark(
      # "R" = { set.seed(123); cpp4rsampling::rejection_sampling(n_samples, mu = 0, sigma = 1, lower = -1.5, upper = 1.5) },
      "cpp4r" = {
        set.seed(123)
        cpp4rsampling::rejection_sampling_cpp4r(n_samples, mu = 0, sigma = 1, lower = -1.5, upper = 1.5)
      },
      "cpp11" = {
        set.seed(123)
        cpp11sampling::rejection_sampling_cpp11(n_samples, mu = 0, sigma = 1, lower = -1.5, upper = 1.5)
      },
      "Rcpp" = {
        set.seed(123)
        Rcppsampling::rejection_sampling_Rcpp(n_samples, mu = 0, sigma = 1, lower = -1.5, upper = 1.5)
      },
      iterations = 10
    )

    d$len <- n_samples

    d
  }
)

rejection_bench[, c("expression", "median", "mem_alloc", "len")]

saveRDS(rejection_bench, "vignettes/rejection_bench.rds", compress = "xz")
