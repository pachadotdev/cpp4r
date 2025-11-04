#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
compiler=${2:-gcc}

echo "Preparing benchmark for $std with $compiler"

# Map to human-readable C++ standard used in DESCRIPTION
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

pkgs=(cpp4rbenchmark cpp11benchmark Rcppbenchmark)
for pkg in "${pkgs[@]}"; do
	echo "- Preparing package: $pkg"
	if [ -d "./${pkg}/src" ]; then
		# write CXX_STD into package Makevars
		echo "CXX_STD = $std" > "./${pkg}/src/Makevars.in"
	fi

	# update DESCRIPTION placeholder C++NN -> actual (if file exists)
	if [ -f "./${pkg}/DESCRIPTION" ]; then
		sed -i "s/C++NN/${cpp_std}/" "./${pkg}/DESCRIPTION" || true
	fi

	# update run.R placeholder CXXNN -> actual (if exists)
	if [ -f "./${pkg}/bench/run.R" ]; then
		sed -i "s/CXXNN/${std}/" "./${pkg}/bench/run.R" || true
	fi
done

echo "Prepared."

# Embed cpp_std and cpp_compiler into central benchmark.R for reproducible output names
if [ -f ./benchmark.R ]; then
	# Replace the cpp_std assignment line with the full CXX token (e.g., CXX17)
	sed -E -i "s/^[[:space:]]*cpp_std[[:space:]]*<-[[:space:]]*\".*\"/cpp_std <- \"${std}\"/" benchmark.R || true
	# Replace the cpp_compiler assignment line
	sed -E -i "s/^[[:space:]]*cpp_compiler[[:space:]]*<-[[:space:]]*\".*\"/cpp_compiler <- \"${compiler}\"/" benchmark.R || true
fi

# Configure Makevars for the selected compiler
makevars="$HOME/.R/Makevars"
if [ -f "$makevars" ]; then
	if [ "$compiler" = "clang" ]; then
		echo "Configuring Makevars for Clang"
		# Uncomment Clang flags section
		sed -i 's/^# CC = clang/CC = clang/' "$makevars"
		sed -i 's/^# CXX = clang++/CXX = clang++/' "$makevars"
		sed -i 's/^# CXX11 = clang++/CXX11 = clang++/' "$makevars"
		sed -i 's/^# CXX14 = clang++/CXX14 = clang++/' "$makevars"
		sed -i 's/^# CXX17 = clang++/CXX17 = clang++/' "$makevars"
		sed -i 's/^# CXX20 = clang++/CXX20 = clang++/' "$makevars"
		sed -i 's/^# CXX23 = clang++/CXX23 = clang++/' "$makevars"
		sed -i 's/^# SHLIB_CXXLD = clang++/SHLIB_CXXLD = clang++/' "$makevars"
	else
		echo "Configuring Makevars for GCC"
		# Comment out Clang flags section
		sed -i 's/^CC = clang/# CC = clang/' "$makevars"
		sed -i 's/^CXX = clang++/# CXX = clang++/' "$makevars"
		sed -i 's/^CXX11 = clang++/# CXX11 = clang++/' "$makevars"
		sed -i 's/^CXX14 = clang++/# CXX14 = clang++/' "$makevars"
		sed -i 's/^CXX17 = clang++/# CXX17 = clang++/' "$makevars"
		sed -i 's/^CXX20 = clang++/# CXX20 = clang++/' "$makevars"
		sed -i 's/^CXX23 = clang++/# CXX23 = clang++/' "$makevars"
		sed -i 's/^SHLIB_CXXLD = clang++/# SHLIB_CXXLD = clang++/' "$makevars"
	fi
fi
