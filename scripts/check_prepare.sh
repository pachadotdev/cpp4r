#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}

echo "Preparing benchmark for $std"
# write CXX_STD into package Makevars
echo "CXX_STD = $std" > ./cpp4rtest/src/Makevars.in

# update DESCRIPTION to set C++ standard placeholder C++NN -> actual
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

sed -i "s/C++NN/${cpp_std}/" ./cpp4rtest/DESCRIPTION

echo "Prepared."
