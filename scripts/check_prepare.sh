#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
compiler=${2:-gcc}

echo "==============================="
echo "Preparing C++ code with $std standard and $compiler compiler"
echo ""

# Note: USE_CLANG should be set by the calling script (check_loop.sh)
# and will be read by Makevars during R CMD INSTALL

# Set the C++ standard in DESCRIPTION
sed -i -E "s|CXX_STD = CXX[0-9]{2}|CXX_STD = $std|" DESCRIPTION
