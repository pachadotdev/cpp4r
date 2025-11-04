make_bench_data <- function(n, prefix = "bench_data") {
    set.seed(123L)

    # Numeric vectors
    x <- rnorm(n)
    y <- rnorm(n)
    w <- runif(n)

    # Matrices: create matrices with total elements approximately n.
    # For A, choose rows = floor(sqrt(n)) and cols = ceil(n / rows) so rows*cols ~ n.
    rows_A <- as.integer(floor(sqrt(n)))
    cols_A <- as.integer(ceiling(n / rows_A))
    A <- matrix(rnorm(rows_A * cols_A), nrow = rows_A, ncol = cols_A)
    # For B, choose inner dimension = cols_A and a modest number of output columns (10)
    B <- matrix(rnorm(cols_A * 10L), nrow = cols_A, ncol = 10L)

    # Data frame with numeric and non-numeric columns
    df <- data.frame(
        id = seq_len(n),
        v1 = rnorm(n),
        v2 = rnorm(n, sd = 2),
        category = sample(letters, n, replace = TRUE),
        stringsAsFactors = FALSE
    )

    # Strings vector for pattern matching
    strs <- vapply(
        seq_len(n),
        function(i) paste0(sample(letters, 20, TRUE), collapse = ""),
        FUN.VALUE = ""
    )

    # Group vector: create ~100 groups (or fewer if n < 100)
    n_groups <- min(100L, n)
    groups <- sample(seq_len(n_groups), n, replace = TRUE)

    # For pairwise distance: full n x n matrix is infeasible for very large n.
    # We'll return a matrix for n <= 2000, otherwise return a subsampled matrix of 2000 rows.
    pairwise_x <- if (n <= 2000L) {
        matrix(rnorm(n * 5L), nrow = n, ncol = 5L)
    } else {
        message(
            "n = ",
            n,
            ": pairwise distance full matrix would be too large; returning subsample of 2000 rows"
        )
        matrix(rnorm(2000L * 5L), nrow = 2000L, ncol = 5L)
    }

    list(
        x = x,
        y = y,
        w = w,
        A = A,
        B = B,
        df = df,
        strs = strs,
        groups = groups,
        pairwise_x = pairwise_x
    )
}

# Sizes requested
sizes <- list(
    small = 1e3,
    medium = 1e4,
    large = 1e5
)

out_dir <- dirname(sys.frame(1)$ofile %||% "./")

for (nm in names(sizes)) {
    n <- as.integer(sizes[[nm]])
    dat <- make_bench_data(n)
    file <- file.path("./", paste0("bench_data_", nm, ".rds"))
    saveRDS(dat, file = file)
    message("Wrote: ", file, " (n = ", n, ")")
}
