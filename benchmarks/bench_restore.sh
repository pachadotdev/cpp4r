#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
compiler=${2:-gcc}

echo "Restoring files for $std with $compiler"

# Convert std to C++ format for DESCRIPTION (e.g., CXX20 -> C++20)
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

pkgs=(cpp4rbenchmark cpp11benchmark Rcppbenchmark)
for pkg in "${pkgs[@]}"; do
	echo "- Restoring: $pkg"
	if [ -d "./${pkg}/src" ]; then
		echo "CXX_STD = CXXNN" > "./${pkg}/src/Makevars.in"
	fi

	if [ -f "./${pkg}/DESCRIPTION" ]; then
		sed -i "s/${cpp_std}/C++NN/" "./${pkg}/DESCRIPTION" || true
	fi

	if [ -f "./${pkg}/bench/run.R" ]; then
		sed -i "s/${std}/CXXNN/" "./${pkg}/bench/run.R" || true
	fi
done

# Restore benchmark.R placeholders
if [ -f ./benchmark.R ]; then
	echo "Reverting cpp_std and cpp_compiler assignments in benchmark.R to placeholders"
	sed -E -i "s/^[[:space:]]*cpp_std[[:space:]]*<-[[:space:]]*\".*\"/cpp_std <- \"CXXNN\"/" benchmark.R || true
	sed -E -i "s/^[[:space:]]*cpp_compiler[[:space:]]*<-[[:space:]]*\".*\"/cpp_compiler <- \"XYZ\"/" benchmark.R || true
fi
