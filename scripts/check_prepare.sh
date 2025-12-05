#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
compiler=${2:-gcc}

echo "==============================="
echo "Preparing C++ code with $std standard and $compiler compiler"
echo ""

chmod +x ./extended-tests/cpp4rtest/configure
chmod +x ./extended-tests/cpp4rtest/cleanup
