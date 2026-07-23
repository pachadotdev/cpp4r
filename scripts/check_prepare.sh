#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX17}
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
compiler=${2:-gcc}

echo "==============================="
echo "Preparing C++ code with $std standard and $compiler compiler"
echo ""

# Regenerate cpp4rtest/src/Makevars from Makevars.in, pinned to $std, via
# the package's own configure script (the same one R CMD INSTALL runs from a
# tarball) so this local check exercises the same code path as check.sh.
(cd ./cpp4rtest && cpp4rtest_CXX_STD="$std" ./configure)
