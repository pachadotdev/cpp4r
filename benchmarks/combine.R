library(purrr)
library(dplyr)
library(ggplot2)

# helper: format numeric in plain scientific notation like '8.72 x 10^-20'
fmt_sci <- function(x, digits = 3) {
    # vectorized formatter: returns character vector
    x <- as.numeric(x)
    out <- character(length(x))
    na_idx <- is.na(x) | !is.finite(x)
    out[na_idx] <- as.character(x[na_idx])
    ok_idx <- !na_idx
    if (any(ok_idx)) {
        xs <- signif(x[ok_idx], digits = digits)
        # handle zeros
        zero_idx <- xs == 0
        if (any(zero_idx)) {
            out[which(ok_idx)[zero_idx]] <- "0"
        }
        nonzero_idx <- !zero_idx
        if (any(nonzero_idx)) {
            vals <- xs[nonzero_idx]
            e <- floor(log10(abs(vals)))
            m <- vals / (10^e)
            m_str <- sub("\\.?0+$", "", format(m, scientific = FALSE, digits = digits))
            out_idx <- which(ok_idx)[nonzero_idx]
            out[out_idx] <- paste0(m_str, " x 10^", e)
        }
    }
    out
}

finp <- list.files(pattern = "bench_results_small_*", full.names = TRUE, recursive = TRUE)

res <- map_df(
    finp,
    function(f) {
        d <- readRDS(f)
        d <- map_df(
            seq_along(d),
            function(x) {
                d[[x]] %>%
                    mutate(test = names(d)[x])
            }
        )
        d$finp <- f
        as_tibble(d)
    }
)

res <- res %>%
    mutate(
        cpp_standard = case_when(
            grepl("CXX11", finp) ~ "C++11",
            grepl("CXX14", finp) ~ "C++14",
            grepl("CXX17", finp) ~ "C++17",
            grepl("CXX20", finp) ~ "C++20",
            grepl("CXX23", finp) ~ "C++23",
            TRUE ~ NA_character_
        ),
        cpp_compiler = case_when(
            grepl("gcc", finp) ~ "GCC",
            grepl("clang", finp) ~ "Clang",
            TRUE ~ NA_character_
        )
    ) %>%
    select(cpp_standard, cpp_compiler, test, backend, mem_alloc, median)

unique(res$test)

# > unique(res$test)
#  [1] "matrix_manip"      "matrix_power"      "sort"
#  [4] "crossprod"         "linear_regression" "determinant"
#  [7] "fibonacci_vector"  "hilbert_matrix"    "gcd_vector"
# [10] "toeplitz_matrix"   "escoufier"

# R BENCHMARK PRESENTATION

#    R Benchmark 2.5
#    ===============
# Number of times each test is run__________________________:  3

#    I. Matrix calculation
#    ---------------------
# Creation, transp., deformation of a 2500x2500 matrix (sec):  1.10533333333333 
# 2400x2400 normal distributed random matrix ^1000____ (sec):  0.986333333333334 
# Sorting of 7,000,000 random values__________________ (sec):  1.044 
# 2800x2800 cross-product matrix (b = a' * a)_________ (sec):  1.02333333333333 
# Linear regr. over a 3000x3000 matrix (c = a \ b')___ (sec):  0.873666666666665 
#                       --------------------------------------------
#                  Trimmed geom. mean (2 extremes eliminated):  1.01760783769740 

#    II. Matrix functions
#    --------------------
# FFT over 2,400,000 random values____________________ (sec):  1.175 
# Eigenvalues of a 640x640 random matrix______________ (sec):  1.18100000000000 
# Determinant of a 2500x2500 random matrix____________ (sec):  1.024 
# Cholesky decomposition of a 3000x3000 matrix________ (sec):  1.22600000000000 
# Inverse of a 1600x1600 random matrix________________ (sec):  1.04733333333333 
#                       --------------------------------------------
#                 Trimmed geom. mean (2 extremes eliminated):  1.13272433303719 

#    III. Programmation
#    ------------------
# 3,500,000 Fibonacci numbers calculation (vector calc)(sec):  1.14866666666667 
# Creation of a 3000x3000 Hilbert matrix (matrix calc) (sec):  0.984666666666667 
# Grand common divisors of 400,000 pairs (recursion)__ (sec):  1.0960 
# Creation of a 500x500 Toeplitz matrix (loops)_______ (sec):  1.13266666666667 
# Escoufier's method on a 45x45 matrix (mixed)________ (sec):  1.03999999999999 
#                       --------------------------------------------
#                 Trimmed geom. mean (2 extremes eliminated):  1.08888497297556 


# Total time for all 15 tests_________________________ (sec):  16.088 
# Overall mean (sum of I, II and III trimmed means/3)_ (sec):  1.07868728433365 
#                       --- End of test ---

res <- res %>%
    mutate(
        benchmark = case_when(
            test == "matrix_manip" ~ "I. Matrix calculation",
            test == "matrix_power" ~ "I. Matrix calculation",
            test == "sort" ~ "I. Matrix calculation",
            test == "crossprod" ~ "I. Matrix calculation",
            test == "linear_regression" ~ "I. Matrix calculation",

            test == "determinant" ~ "II. Matrix functions",

            test == "fibonacci_vector" ~ "III. Programmation",
            test == "hilbert_matrix" ~ "III. Programmation",
            test == "gcd_vector" ~ "III. Programmation",
            test == "toeplitz_matrix" ~ "III. Programmation",
            test == "escoufier" ~ "III. Programmation",
            TRUE ~ NA_character_
        )
    )

res <- res %>%
    mutate(cpp_std_co = paste(cpp_standard, cpp_compiler, sep = " - ")) %>%
    mutate_if(is.character, as.factor)

res2 <- res %>%
    select(-cpp_standard, -cpp_compiler) %>%
    group_by(test, cpp_std_co) %>%
    mutate_if(is.numeric, ~ . / min(.))

res3 <- res %>%
    select(backend, test, mem_alloc) %>%
    distinct()



# summarise fastest std/compiler per backend and test
res <- res %>%
    select(cpp_std_co, backend, test, mem_alloc, min, median, max) %>%
    group_by(test, backend) %>%
    summarise(
        mem_alloc = unique(mem_alloc),
        best_min_cpp_std = cpp_std_co[which.min(min)],
        best_min = min[which.min(median)],
        worst_max_cpp_std = cpp_std_co[which.min(max)],
        worst_max = max[which.min(max)]
    ) %>%
    group_by(test) %>%
    mutate(
        relative_mem_alloc = 100 * mem_alloc / min(mem_alloc),
        relative_best_min = 100 * best_min / min(best_min),
        relative_worst_max = 100 * worst_max / min(worst_max)
    ) %>%
    mutate(
        mem_alloc = paste(signif(mem_alloc, 2), "MB"),
        best_time = paste0(fmt_sci(best_min, digits = 3), "s (", best_min_cpp_std, ")"),
        worst_time = paste0(fmt_sci(worst_max, digits = 3), "s (", worst_max_cpp_std, ")"),
        relative_mem_alloc = paste0(format(signif(relative_mem_alloc, 2), big.mark = ",", small.mark = "."), "%"),
        relative_best_min = paste0(format(signif(relative_best_min, 2), big.mark = ",", small.mark = "."), "%"),
        relative_worst_max = paste0(format(signif(relative_worst_max, 2), big.mark = ",", small.mark = "."), "%")
    ) %>%
    select(test, backend, mem_alloc, best_time, worst_time, relative_mem_alloc, relative_best_min, relative_worst_max)

knitr::kable(res)
