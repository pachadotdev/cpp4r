#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
compiler=${2:-gcc}

echo "==============================="
echo "Preparing C++ code with $std standard and $compiler compiler"
echo ""

# Set compiler via environment variable for Make
if [ "$compiler" = "clang" ]; then
  export USE_CLANG=1
else
  unset USE_CLANG
fi

# Install dependencies
Rscript -e "install.packages('cpp11', repos = 'https://cloud.r-project.org')"
Rscript -e "install.packages('Rcpp', repos = 'https://cloud.r-project.org')"

Rscript -e "install.packages('cpp4r', repos = NULL, type = 'source')"

# Set the C++ standard in DESCRIPTION
sed -i -E "s|CXX_STD = CXX[0-9]{2}|CXX_STD = $std|" DESCRIPTION
