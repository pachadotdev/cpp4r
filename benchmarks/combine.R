library(purrr)
library(dplyr)
library(ggplot2)

finp <- list.files(pattern = "bench_results*")

res <- map_df(
    finp,
    function(f) {
        d <- readRDS(f)
        d <- map_df(
            seq_along(d),
            function(x) {
                d[[x]] %>%
                    mutate(
                        test = names(d)[x]                    )
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
    select(cpp_standard, cpp_compiler, test, backend, mem_alloc, min:max)

unique(res$test)

res <- res %>%
    mutate(
        test = case_when(
            test == "prealloc_weighted_sum" ~ "Preallocated Weighted Sum",
            test == "matrix_multiply" ~ "Matrix Multiplication",
            test == "rolling_mean" ~ "Rolling Mean",
            test == "dataframe_summary" ~ "Data Frame Summary",
            test == "string_pattern_count" ~ "String Pattern Count",
            test == "grouped_mean" ~ "Grouped Mean",
            test == "pairwise_distances" ~ "Pairwise Distances",
            test == "cumulative_ops" ~ "Cumulative Operations",
            test == "bootstrap_mean" ~ "Bootstrap Mean",
            TRUE ~ test
        )
    )

knitr::kable(res)

res <- res %>%
    mutate(cpp_std_co = paste(cpp_standard, cpp_compiler, sep = " - ")) %>%
    mutate_if(is.character, as.factor)

res2 <- res %>%
    select(-cpp_standard, -cpp_compiler) %>%
    group_by(test, cpp_std_co) %>%
    mutate_if(is.numeric, ~ . / min(.))

ggplot(res, aes(x = backend)) +
    # geom_errorbar(
    #     aes(
    #         ymin = min,
    #         ymax = max
    #     ),
    #     width = 0.5,
    #     position = position_dodge(width = 0.5)
    # ) +
    geom_col(
        aes(
            y = median,
            fill = cpp_std_co
        ),
        position = position_dodge()
    ) +
    facet_wrap(~test, scales = "free_y") +
    theme_minimal() +
    labs(
        title = "Benchmark Results Across Backends and C++ Standards",
        x = "Backend",
        y = "Median Execution Time (seconds)",
        fill = "C++ Standard - Compiler"
    ) +
    # scale_color_viridis_d()
    scale_fill_manual(values = tintin::tintin_pal()(10))

ggplot(res2, aes(x = backend)) +
    geom_col(
        aes(
            y = median,
            fill = cpp_std_co
        ),
        position = position_dodge()
    ) +
    facet_wrap(~test, scales = "free_y") +
    theme_minimal() +
    labs(
        title = "Benchmark Results Across Backends and C++ Standards",
        x = "Backend",
        y = "Median Execution Time (Normalized)",
        fill = "C++ Standard - Compiler"
    ) +
    # scale_color_viridis_d()
    scale_fill_manual(values = tintin::tintin_pal()(10))

res3 <- res %>%
    select(backend, test, mem_alloc) %>%
    distinct()

ggplot(res3, aes(x = backend)) +
    geom_col(
        aes(
            y = mem_alloc
        ),
        fill = tintin::tintin_clrs()[2],
        position = position_dodge()
    ) +
    facet_wrap(~test, scales = "free_y") +
    theme_minimal() +
    labs(
        title = "Benchmark Results Across Backends and C++ Standards",
        x = "Backend",
        y = "Memory Allocation (MB)"
    )
