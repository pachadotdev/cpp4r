#!/usr/bin/env bash
set -euo pipefail

# Accept std as first arg, default to CXX11
std=${1:-CXX11}
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
compiler=${2:-gcc}

echo "Restoring files for $std and $compiler"

# Convert std to C++ format for DESCRIPTION (e.g., CXX20 -> C++20)
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

# Restore Makevars.in to defaults
sed -i 's/^CXX_STD = .*/CXX_STD = CXX23/' ./cpp4rtest/src/Makevars.in
# Remove any leftover clang block from a previous prepare run
sed -i '/# cpp4r-check-clang-begin/,/# cpp4r-check-clang-end/d' ./cpp4rtest/src/Makevars.in

# Remove generated Makevars
rm -f "./cpp4rtest/src/Makevars"

# Restore GCC by unsetting USE_CLANG
unset USE_CLANG || true

# Clear check files
rm -rf ./cpp4rtest.Rcheck || true
