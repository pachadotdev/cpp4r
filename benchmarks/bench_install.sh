#!/usr/bin/env bash
set -euo pipefail

echo "Cleaning, documenting and installing benchmark packages"

Rscript -e 'pkgs <- c("cpp4rbenchmark","cpp11benchmark","Rcppbenchmark"); for (p in pkgs) { if (dir.exists(p)) {
 message("Processing: ", p); try(devtools::clean_dll(p), silent=TRUE);
 try(devtools::document(p), silent=TRUE);
 try(devtools::install(p, upgrade = "never"), silent=TRUE) } }'

echo "Install complete."
