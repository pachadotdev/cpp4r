#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}

echo "Restoring files for $std"

# Convert std to C++ format for DESCRIPTION (e.g., CXX20 -> C++20)
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

# Restore Makevars.in placeholder
echo "CXX_STD = CXXNN" > ./cpp4rbenchmark/src/Makevars.in

# Restore DESCRIPTION placeholder (needs C++ format)
sed -i "s/${cpp_std}/C++NN/" ./cpp4rbenchmark/DESCRIPTION || true

# Restore run.R placeholder (needs CXX format)
sed -i "s/${std}/CXXNN/" ./cpp4rbenchmark/bench/run.R || true
