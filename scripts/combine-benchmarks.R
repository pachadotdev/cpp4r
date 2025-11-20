library(purrr)
library(dplyr)
library(tidyr)
library(ggplot2)

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

# add cpp4/cpp11 time and cpp4r/Rcpp time ratios (compute while values are numeric)
res <- res %>%
    mutate(
        rel_cpp4r_cpp11 = signif(rel_time_cpp4r / rel_time_cpp11, 3),
        rel_cpp4r_Rcpp = signif(rel_time_cpp4r / rel_time_Rcpp, 3)
    )

# add rel time for cpp4r within the same test group (numeric)
res <- res %>%
    group_by(test) %>%
    mutate(
        rel_cpp4r_within_test = signif(rel_time_cpp4r / min(rel_time_cpp4r, na.rm = TRUE), 3)
    ) %>%
    ungroup()

format_small <- function(x, digits = 2, threshold = 0.01) {
    # x: numeric vector. For |x| in (0, threshold) produce scientific 'e' format
    xnum <- as.numeric(x)
    out <- character(length(xnum))
    na_idx <- is.na(xnum) | !is.finite(xnum)
    out[na_idx] <- as.character(xnum[na_idx])
    ok_idx <- !na_idx
    if (any(ok_idx)) {
        vals <- xnum[ok_idx]
        out_idx <- which(ok_idx)
        is_zero <- vals == 0
        is_small <- (!is_zero) & (abs(vals) < threshold)
        if (any(is_small)) {
            out[out_idx[is_small]] <- format(vals[is_small], scientific = TRUE, digits = digits)
        }
        if (any(!is_small & !is_zero)) {
            out[out_idx[!is_small & !is_zero]] <- as.character(signif(vals[!is_small & !is_zero], digits = digits))
        }
        if (any(is_zero)) out[out_idx[is_zero]] <- "0"
    }
    out
}

res <- res %>%
    mutate(
        avg_time_cpp11 = format_small(avg_time_cpp11),
        avg_time_cpp4r = format_small(avg_time_cpp4r),
        avg_time_Rcpp = format_small(avg_time_Rcpp),
        rel_time_cpp11 = format_small(rel_time_cpp11),
        rel_time_cpp4r = format_small(rel_time_cpp4r),
        rel_time_Rcpp = format_small(rel_time_Rcpp)
    )

# (ratios computed earlier before formatting)

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
write_latex_table <- function(df, out_file = "./extended-tests-results/bench_summary.tex", arraystretch = 1.2, top_padding_pt = 6) {
    # (old two_line helper removed — cells are constructed inline now)

    # Ensure test grouping order
    df <- df %>% arrange(test, cpp_standard, cpp_compiler)

    # Unique tests
    tests <- unique(df$test)

    # Open output file for writing all tables
    con <- file(out_file, open = "w")
    # Request makecell and multirow packages. If this .tex is included into a larger
    # document, prefer loading these packages in the main preamble instead of here.
    # writeLines(c(
    #     "% Auto-generated benchmark summary",
    #     "% Ensure makecell and multirow packages are available for cell padding and vertical spanning",
    #     "\\RequirePackage{makecell}",
    #     "\\RequirePackage{multirow}"
    # ), con)

    # Helper to extract time and rel parts from "time (rel)" strings
    extract_parts <- function(cell) {
        if (is.na(cell) || nchar(cell) == 0) return(c("", ""))
        m <- regmatches(cell, regexec('^(\\S+)\\s*\\(([^)]+)\\)$', cell))[[1]]
        if (length(m) == 3) return(c(m[2], m[3]))
        return(c(cell, ""))
    }

    for (t in tests) {
        sub <- df %>% filter(test == t)

        # Set per-table cell gap (inner padding). Use the provided top_padding_pt.
        # writeLines(sprintf("\\setcellgapes{%dpt}\\makegapedcells", as.integer(top_padding_pt)), con)

        # Begin a table for this benchmark (no Test column)
        # Use a local group to set \arraystretch (row vertical spacing)
        writeLines(c(
            "\\begin{table}[H]",
            "\\centering",
            sprintf("\\begingroup\\renewcommand{\\arraystretch}{%s}", arraystretch),
            "\\begin{tabular}{|l|l|c|c|c|}",
            "\\hline",
            "Standard & Compiler & cpp11 & cpp4r & Rcpp \\\\",
            "\\hline"
        ), con)

    # New layout: one physical row per compiler
    # Each backend cell will contain "time (rel)" on the same line using \makecell
    # Multirow spans: Standard spans n_compilers rows; Compiler spans 1 row
    back_order <- c("cpp11", "cpp4r", "Rcpp")

    # collect a table of values per (standard, compiler) into a small list
    entries <- purrr::map(seq_len(nrow(sub)), function(ii) {
        row <- sub[ii, ]
        vals_time <- list()
        vals_rel <- list()
        for (b in back_order) {
            colname <- paste0("avg_time_", b)
            if (!is.null(row[[colname]]) && !is.na(row[[colname]]) && nchar(as.character(row[[colname]])) > 0) {
                parts <- extract_parts(row[[colname]])
                vals_time[[b]] <- parts[1]
                vals_rel[[b]] <- parts[2]
            } else {
                vals_time[[b]] <- ""
                vals_rel[[b]] <- ""
            }
        }
        list(cpp_standard = row$cpp_standard, cpp_compiler = row$cpp_compiler, time = vals_time, rel = vals_rel)
    })

    # Now write rows: one row per compiler entry
    # compute how many compiler entries per standard to set multirow spans
    std_table <- tibble::tibble(cpp_standard = sapply(entries, `[[`, "cpp_standard")) %>%
        dplyr::group_by(cpp_standard) %>% dplyr::summarise(n_compilers = dplyr::n()) %>% ungroup()

    for (idx in seq_along(entries)) {
        e <- entries[[idx]]
        n_compilers <- std_table$n_compilers[std_table$cpp_standard == e$cpp_standard]
        std_span <- as.integer(n_compilers)

        # Standard cell only at the first appearance of this standard
        first_std <- TRUE
        if (idx > 1 && entries[[idx - 1]]$cpp_standard == e$cpp_standard) first_std <- FALSE

        std_cell <- if (first_std) {
            if (std_span > 1) sprintf("\\multirow{%d}{*}{%s}", std_span, e$cpp_standard) else e$cpp_standard
        } else ""

        # Compiler cell spans exactly 1 row now
        comp_cell <- sprintf("\\multirow{1}{*}{%s}", e$cpp_compiler)

        # Build cells that contain "time (rel)" on the same line using \makecell
        time_cells <- vapply(back_order, function(b) {
            tval <- e$time[[b]]
            rval <- e$rel[[b]]
            if ((is.null(tval) || nchar(tval) == 0) && (is.null(rval) || nchar(rval) == 0)) return("")
            # escape percent signs (produce literal \% for LaTeX)
            tval <- if (is.null(tval) || nchar(tval) == 0) "" else gsub("%", "\\\\%", tval, fixed = TRUE)
            rval <- if (is.null(rval) || nchar(rval) == 0) "" else gsub("%", "\\\\%", rval, fixed = TRUE)
            if (nchar(rval) == 0) return(tval)
            # sprintf("\\makecell{%s (%s)}", tval, rval)
            sprintf("%s (%s)", tval, rval)
        }, FUN.VALUE = "")

        # replace e-A with \times 10^{-A}
        time_cells <- gsub("e([-+]?\\d+)", " \\\\times 10^{\\1}", time_cells)

        # normalize exponents: remove leading zeros and any leading '+' inside braces
        # examples: {+06} -> {6}, {06} -> {6}, {-06} -> {-6}
        # First remove a leading '+' sign inside the braces
        time_cells <- gsub("\\^\\{\\+([0-9]+)\\}", "^{\\1}", time_cells)
        # Then remove leading zeros (but keep a possible leading '-')
        time_cells <- gsub("\\^\\{(-?)0+([0-9]+)\\}", "^{\\1\\2}", time_cells)

        # $ cell $
        time_cells <- paste0("$", time_cells, "$")

        # safer construction of the LaTeX row
        line <- paste(std_cell, comp_cell, time_cells[1], time_cells[2], time_cells[3], sep = " & ")
        # append LaTeX linebreak \\ (escaped for R string)
        line <- paste0(line, " \\\\")
        writeLines(line, con)

        # Decide which horizontal rule to print after this compiler row.
        next_same_std <- (idx < length(entries) && entries[[idx + 1]]$cpp_standard == e$cpp_standard)
        if (next_same_std) {
            writeLines("\\cline{2-5}", con)
        } else {
            writeLines("\\hline", con)
        }
    }

    # Caption per-benchmark
    caption <- sprintf("%s benchmark", t)
    writeLines(c("\\end{tabular}", sprintf("\\caption{%s}", caption), "\\endgroup"), con)
    writeLines("\\end{table}", con)
        writeLines("", con)
    }

    close(con)
}

# call LaTeX export
write_latex_table(res)
