make_bench_data <- function(n, prefix = "bench_data") {
    set.seed(123L)
    
	# Numeric vectors
	x <- rnorm(n)
	y <- rnorm(n)
	w <- runif(n)

	# Matrices: choose a modest number of columns for matrix tests to keep memory reasonable
	# Use k = min(50, n) columns but cap at 50 for large n
	k <- min(50L, n)
	A <- matrix(rnorm(n * k), nrow = n, ncol = k)
	B <- matrix(rnorm(k * 10L), nrow = k, ncol = 10L) # result will be n x 10

	# Data frame with numeric and non-numeric columns
	df <- data.frame(
		id = seq_len(n),
		v1 = rnorm(n),
		v2 = rnorm(n, sd = 2),
		category = sample(letters, n, replace = TRUE),
		stringsAsFactors = FALSE
	)

	# Strings vector for pattern matching
	strs <- vapply(seq_len(n), function(i) paste0(sample(letters, 20, TRUE), collapse = ""), FUN.VALUE = "")

	# Group vector: create ~100 groups (or fewer if n < 100)
	n_groups <- min(100L, n)
	groups <- sample(seq_len(n_groups), n, replace = TRUE)

	# For pairwise distance: full n x n matrix is infeasible for very large n.
	# We'll return a matrix for n <= 2000, otherwise return a subsampled matrix of 2000 rows.
	pairwise_x <- if (n <= 2000L) {
		matrix(rnorm(n * 5L), nrow = n, ncol = 5L)
	} else {
		message("n = ", n, ": pairwise distance full matrix would be too large; returning subsample of 2000 rows")
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
	# small = 1e3,
	medium = 1e4
	# large = 1e5
)

out_dir <- dirname(sys.frame(1)$ofile %||% "./")

for (nm in names(sizes)) {
	n <- as.integer(sizes[[nm]])
	dat <- make_bench_data(n)
	file <- file.path("./", paste0("bench_data_", nm, ".rds"))
	saveRDS(dat, file = file)
	message("Wrote: ", file, " (n = ", n, ")")
}
