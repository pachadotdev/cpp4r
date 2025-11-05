# Generate data for R-benchmark-25 inspired tests
# Based on R Benchmark 2.5 by Simon Urbanek

set.seed(42)

# Factory to produce datasets for sizes: "small", "medium", "large"
make_bench_data <- function(size = c("small", "medium", "large")) {
  size <- match.arg(size)

  # parameterize values per size to avoid repetition
  params <- switch(size,
    small = list(
      mm_n = 1000, mm_div = 10,
      mp_n = 1000, mp_div = 2, mp_exp = 100,
      sort_n = 1000000,
      cp_n = 1000,
      lr_nrow = 500, lr_ncol = 100,
      det_n = 500,
      fib_n = 100000, fib_scale = 100,
      hilbert_n = 500,
      gcd_n = 50000, gcd_scale = 1000,
      toeplitz_n = 100,
      esc_n = 20
    ),
    medium = list(
      mm_n = 2500, mm_div = 10,
      mp_n = 2500, mp_div = 2, mp_exp = 1000,
      sort_n = 7000000,
      cp_n = 2800,
      lr_nrow = 2000, lr_ncol = 200,
      det_n = 2500,
      fib_n = 3500000, fib_scale = 1000,
      hilbert_n = 3000,
      gcd_n = 400000, gcd_scale = 1000,
      toeplitz_n = 500,
      esc_n = 45
    ),
    large = list(
      mm_n = 3500, mm_div = 10,
      mp_n = 3000, mp_div = 2, mp_exp = 1000,
      sort_n = 10000000,
      cp_n = 3500,
      lr_nrow = 3000, lr_ncol = 300,
      det_n = 3000,
      fib_n = 5000000, fib_scale = 1000,
      hilbert_n = 4000,
      gcd_n = 600000, gcd_scale = 1000,
      toeplitz_n = 800,
      esc_n = 60
    )
  )

  # Unpack for readability
  mm_n <- params$mm_n; mm_div <- params$mm_div
  mp_n <- params$mp_n; mp_div <- params$mp_div; mp_exp <- params$mp_exp
  sort_n <- params$sort_n
  cp_n <- params$cp_n
  lr_nrow <- params$lr_nrow; lr_ncol <- params$lr_ncol
  det_n <- params$det_n
  fib_n <- params$fib_n; fib_scale <- params$fib_scale
  hilbert_n <- params$hilbert_n
  gcd_n <- params$gcd_n; gcd_scale <- params$gcd_scale
  toeplitz_n <- params$toeplitz_n
  esc_n <- params$esc_n

  # Build the dataset single time
  list(
    # I. Matrix Calculation
    matrix_manip_vec = rnorm(mm_n * mm_n) / mm_div,
    matrix_manip_nrow = mm_n,
    matrix_manip_ncol = mm_n,

    matrix_power = abs(matrix(rnorm(mp_n * mp_n) / mp_div, ncol = mp_n, nrow = mp_n)),
    power_exponent = mp_exp,

    sort_vec = rnorm(sort_n),

    crossprod_mat = matrix(rnorm(cp_n * cp_n), ncol = cp_n, nrow = cp_n),

    linreg_X = matrix(rnorm(lr_nrow * lr_ncol), ncol = lr_ncol, nrow = lr_nrow),
    linreg_y = rnorm(lr_nrow),

    # II. Matrix Functions
    det_mat = matrix(rnorm(det_n * det_n), ncol = det_n, nrow = det_n),

    # III. Programming
    fib_indices = floor(runif(fib_n) * fib_scale),
    hilbert_n = hilbert_n,
    gcd_x = as.integer(ceiling(runif(gcd_n) * gcd_scale)),
    gcd_y = as.integer(ceiling(runif(gcd_n) * gcd_scale)),
    toeplitz_n = toeplitz_n,
    escoufier_mat = abs(matrix(rnorm(esc_n * esc_n), ncol = esc_n, nrow = esc_n))
  )
}

# Create datasets using the factory
bench_data_small <- make_bench_data("small")
bench_data_medium <- make_bench_data("medium")
bench_data_large <- make_bench_data("large")

# Save datasets
saveRDS(bench_data_small, file = "./bench_data_small.rds", compress = "xz")
saveRDS(bench_data_medium, file = "./bench_data_medium.rds", compress = "xz")
saveRDS(bench_data_large, file = "./bench_data_large.rds", compress = "xz")
