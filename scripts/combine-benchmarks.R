library(purrr)
library(dplyr)
library(tidyr)
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

finp <- list.files(path = "extended-tests-results", pattern = "bench_results_clang|bench_results_gcc", full.names = TRUE, recursive = TRUE)

bench_all <- map_df(
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

# bench_all %>%
#     filter(test == "multi_operation") %>%
#     pull(time)

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
    ) %>%
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
    select(backend, cpp_standard, cpp_compiler, test, time) %>%
    mutate(
        test = case_when(
            test == "add_two" ~ "Add two matrices",
            test == "add_four" ~ "Add four matrices",
            test == "multiply_four" ~ "Multiply four matrices",
            test == "submatrix_manipulation" ~ "Submatrix manipulation",
            test == "multi_operation" ~ "Multiple matrix operations",
            TRUE ~ NA_character_
        ),
        backend = case_when(
            grepl("cpp11", backend) ~ "cpp11",
            grepl("cpp4r", backend) ~ "cpp4r",
            grepl("rcpp", backend) ~ "Rcpp",
            TRUE ~ "base R"
        )
    )

res <- res %>%
    mutate(
        base_r = ifelse(backend == "base R", "base", "cpp")
    ) %>%
    pivot_wider(
        names_from = base_r,
        values_from = time,
        names_prefix = "time_"
    ) %>%
    fill(time_base, .direction = "down") %>%
    drop_na(time_cpp)

res <- res %>%
    mutate(
        avg_time = map_dbl(
            res$time_cpp,
            function(x) {
                y <- sort(x)
                mean(y[2:(length(y) - 1)], na.rm = TRUE)
            }
        )
    ) %>%
    mutate(
        rel_time = map2_dbl(
            res$time_cpp,
            res$time_base,
            function(x, y) {
                # geometric mean excluding extremes, but compute via logs to avoid underflow
                z <- sort(x/y)
                vec <- z[2:(length(z) - 1)]
                vec <- vec[is.finite(vec) & vec > 0]
                if (length(vec) == 0) return(NA_real_)
                exp(mean(log(vec)))
            }
        )
    ) %>%
    select(-time_cpp, -time_base)

res <- res %>%
    pivot_wider(
        names_from = backend,
        values_from = c(avg_time, rel_time)
    )

res <- res %>%
    # filter(benchmark %in% c(
    #     "Multiply four matrices",
    #     "Submatrix manipulation",
    #     "Multiple matrix operations"
    # )) %>%
    arrange(test, cpp_standard, cpp_compiler) %>%
    select(test, everything())

res <- res %>%
    mutate(
        avg_time_cpp11 = signif(avg_time_cpp11, 3),
        avg_time_cpp4r = signif(avg_time_cpp4r, 3),
        avg_time_Rcpp = signif(avg_time_Rcpp, 3),
        rel_time_cpp11 = signif(rel_time_cpp11, 3),
        rel_time_cpp4r = signif(rel_time_cpp4r, 3),
        rel_time_Rcpp = signif(rel_time_Rcpp, 3)
    )

# add cpp4/cpp11 time and cpp4r/Rcpp time ratios
res <- res %>%
    mutate(
        rel_cpp4r_cpp11 = signif(rel_time_cpp4r / rel_time_cpp11, 3),
        rel_cpp4r_Rcpp = signif(rel_time_cpp4r / rel_time_Rcpp, 3)
    )

# add rel time for cpp4r within the same test group
res <- res %>%
    group_by(test) %>%
    mutate(
        rel_cpp4r_within_test = signif(rel_time_cpp4r / min(rel_time_cpp4r, na.rm = TRUE), 3)
    ) %>%
    ungroup()

res <- res %>%
    mutate(
        avg_time_cpp11 = paste(avg_time_cpp11, paste0("(", rel_time_cpp11, ")")),
        avg_time_cpp4r = paste(avg_time_cpp4r, paste0("(", rel_time_cpp4r, ")")),
        avg_time_Rcpp = paste(avg_time_Rcpp, paste0("(", rel_time_Rcpp, ")"))
    ) %>%
    select(-starts_with("rel_time_"))

# write markdown summary
writeLines(knitr::kable(res), "./extended-tests-results/bench_summary.md")

# LaTeX export: create table with division lines and two-line cells
write_latex_table <- function(df, out_file = "./extended-tests-results/bench_summary.tex") {
    # Helper to make two-line cell: time \newline (rel)
    two_line <- function(time_str, rel_str) {
        # Escape percent signs and backslashes
        time_str <- gsub("%", "\\%", time_str, fixed = TRUE)
        rel_str <- gsub("%", "\\%", rel_str, fixed = TRUE)
        # Use \shortstack to stack lines within a cell
        sprintf("\\shortstack{ %s \\\\ (%s) }", time_str, rel_str)
    }

    # Ensure test grouping order
    df <- df %>% arrange(test, cpp_standard, cpp_compiler)

    # Unique tests and counts
    tests <- unique(df$test)

    # Begin LaTeX table
    con <- file(out_file, open = "w")
    writeLines(
        c(
            "% Auto-generated benchmark summary",
            "\\begin{table}[ht]",
            "\\centering",
            "\\begin{tabular}{|l|l|l|c|c|c|}",
            "\\hline",
            "Test & Standard & Compiler & cpp11 & cpp4r & Rcpp \\\\ ",
            "\\hline"
        ), con
    )

    for (t in tests) {
        sub <- df %>% filter(test == t)
        nrows <- nrow(sub)
        first_row <- TRUE
        for (i in seq_len(nrows)) {
            row <- sub[i, ]
            # build two-line strings by splitting the "time (rel)" format
            extract_parts <- function(cell) {
                if (is.na(cell) || nchar(cell) == 0) return(c("", ""))
                # cell format is like "0.00421 (0.664)" or similar
                m <- regmatches(cell, regexec('^(\\S+)\\s*\\(([^)]+)\\)$', cell))[[1]]
                if (length(m) == 3) return(c(m[2], m[3]))
                return(c(cell, ""))
            }

            p11 <- extract_parts(row$avg_time_cpp11)
            p4r <- extract_parts(row$avg_time_cpp4r)
            prc <- extract_parts(row$avg_time_Rcpp)

            cpp11_cell <- if (p11[1] != "") two_line(p11[1], p11[2]) else ""
            cpp4r_cell <- if (p4r[1] != "") two_line(p4r[1], p4r[2]) else ""
            rcpp_cell <- if (prc[1] != "") two_line(prc[1], prc[2]) else ""

            test_cell <- if (first_row) sprintf("\\multirow{%d}{*}{%s}", nrows, t) else ""
            first_row <- FALSE

            line <- sprintf("%s & %s & %s & %s & %s & %s \\\\",
                            test_cell,
                            row$cpp_standard,
                            row$cpp_compiler,
                            cpp11_cell,
                            cpp4r_cell,
                            rcpp_cell)
            writeLines(line, con)
            writeLines("\\hline", con)
        }
    }

    writeLines(c("\\end{tabular}", "\\caption{Benchmark summary}", "\\end{table}"), con)
    close(con)
}

# call LaTeX export
write_latex_table(res)
