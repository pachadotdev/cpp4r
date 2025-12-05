# Check all URLs in the package for validity and redirects
# This script checks URLs in DESCRIPTION, README, vignettes, and documentation

# Install urlchecker if not available
if (!requireNamespace("urlchecker", quietly = TRUE)) {
  install.packages("urlchecker")
}

library(urlchecker)

# Check all URLs in the package
cat("Checking URLs in package...\n")
url_check_results <- urlchecker::url_check(".")

# Display results
if (length(url_check_results) == 0) {
  cat("✅ All URLs are valid!\n")
} else {
  cat("❌ Found issues with URLs:\n")
  print(url_check_results)
}

# Also check for specific redirections that CRAN doesn't like
cat("\n" %in% "Checking for common redirect patterns...\n")

# Check DESCRIPTION file specifically
desc_file <- "DESCRIPTION"
if (file.exists(desc_file)) {
  desc_content <- readLines(desc_file)

  # Look for URLs in DESCRIPTION
  url_lines <- grep("http", desc_content, value = TRUE)

  if (length(url_lines) > 0) {
    cat("URLs found in DESCRIPTION:\n")
    for (line in url_lines) {
      cat("  ", line, "\n")
    }
  }
}

# Check README files
readme_files <- c("README.md", "README.Rmd")
for (readme in readme_files) {
  if (file.exists(readme)) {
    cat(paste0("\nChecking ", readme, "...\n"))
    readme_content <- readLines(readme)
    url_lines <- grep("http", readme_content, value = TRUE)

    if (length(url_lines) > 0) {
      cat("URLs found:\n")
      for (line in url_lines) {
        cat("  ", trimws(line), "\n")
      }
    }
  }
}

cat("\n✨ URL check complete!\n")
