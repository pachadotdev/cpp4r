#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
compiler=${2:-gcc}

echo "==============================="
echo "Preparing C++ code with $std standard and $compiler compiler"
echo ""

# Patch CXX_STD in cpp4rtest/src/Makevars.in
sed -i "s/^CXX_STD = .*/CXX_STD = ${std}/" ./cpp4rtest/src/Makevars.in

chmod +x ./cpp4rtest/configure
chmod +x ./cpp4rtest/cleanup
