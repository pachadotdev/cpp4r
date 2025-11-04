#!/usr/bin/env bash
set -euo pipefail

# Accept std as first arg, default to CXX11
std=${1:-CXX11}
compiler=${2:-gcc}

echo "Restoring files for $std and $compiler"

# Convert std to C++ format for DESCRIPTION (e.g., CXX20 -> C++20)
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

# Restore Makevars.in placeholder
echo "CXX_STD = CXXNN" > ./cpp4rtest/src/Makevars.in

# Restore DESCRIPTION placeholder (needs C++ format)
sed -i "s/${cpp_std}/C++NN/" ./cpp4rtest/DESCRIPTION || true

# Restore GCC by unsetting USE_CLANG
unset USE_CLANG || true

# Clear check files
rm -rf ./cpp4rtest.Rcheck || true
rm cpp4rtest_.*tar.gz || true
