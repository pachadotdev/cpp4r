#!/usr/bin/env bash
set -euo pipefail

echo "Cleaning, documenting and installing cpp4rbenchmark"
Rscript -e 'devtools::clean_dll("cpp4rbenchmark"); cpp4r::register("cpp4rbenchmark"); devtools::document("cpp4rbenchmark");' 
Rscript -e 'devtools::install("cpp4rbenchmark", upgrade = "never")'

echo "Install complete."
