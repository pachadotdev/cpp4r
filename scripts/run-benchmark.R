cpp_compiler <- Sys.getenv("CPP_COMPILER", "gcc")
cpp_std <- Sys.getenv("CPP_STD", "CXX11")

message(sprintf("CPP_COMPILER = %s", cpp_compiler))
message(sprintf("CPP_STD = %s", cpp_std))

library(bench)
library(cpp11benchmark)
library(cpp4rbenchmark)
library(Rcppbenchmark)

n <- 500L
iterations <- 100L
tol <- 1e-9

set.seed(42)
X <- matrix(rnorm(n * n), nrow = n, ncol = n)
XtX <- crossprod(X)  # Symmetric positive semi-definite matrix

# Base R rank-revealing Cholesky implementation (partially vectorized)
rrc <- function(XtX, tol = 1e-9) {
  p <- ncol(XtX)
  excluded <- integer(p)

  if (p == 0L) {
    return(list(excluded = excluded, success = TRUE))
  }

  R <- matrix(0, p, p)
  n_excluded <- 0L

  for (j in seq_len(p)) {
    R_jj <- XtX[j, j]

    if (j > 1L) {
      active <- which(excluded[1:(j - 1L)] == 0L)
      if (length(active) > 0L) {
        R_jj <- R_jj - sum(R[active, j]^2)
      }
    }

    if (R_jj < tol) {
      excluded[j] <- 1L
      n_excluded <- n_excluded + 1L
      next
    }

    R_jj <- sqrt(R_jj)
    R[j, j] <- R_jj
    inv_R_jj <- 1 / R_jj

    if (j < p) {
      cols <- (j + 1L):p
      R_j_cols <- XtX[j, cols]

      if (j > 1L) {
        active <- which(excluded[1:(j - 1L)] == 0L)
        if (length(active) > 0L) {
          # R[active, j] is a vector, R[active, cols] is a matrix
          # We need: for each col, sum(R[active, j] * R[active, col])
          R_j_cols <- R_j_cols - as.vector(R[active, j] %*% R[active, cols, drop = FALSE])
        }
      }

      R[j, cols] <- R_j_cols * inv_R_jj
    }
  }

  list(excluded = excluded, success = n_excluded < p)
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

bench_cpp <- function(iterations = 1L) {
  results <- list()

  results$rrc <- light_bench_results(
    bench::mark(
      rrc(XtX, tol),
      rrc_cpp11(XtX, tol),
      rrc_cpp4r(XtX, tol),
      rrc_rcpp(XtX, tol),
      iterations = 10L
    )
  )

  results
}

fout <- file.path(
  "./extended-tests-results/",
  paste0("bench_results_", cpp_compiler, "_", cpp_std, ".rds")
)

message(sprintf("Output file: %s", fout))

bench_results <- bench_cpp(iterations = iterations)

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
