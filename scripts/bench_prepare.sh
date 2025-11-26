#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
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
	if [ -d "./extended-tests/${pkg}/src" ]; then
		# update or add CXX_STD in package Makevars without clobbering other content
		src_dir="./extended-tests/${pkg}/src"
		infile="${src_dir}/Makevars.in"
		outfile="${src_dir}/Makevars"

		if [ -f "$infile" ]; then
			cp "$infile" "$outfile"
		else
			if [ ! -f "$outfile" ]; then
				touch "$outfile"
			fi
		fi

		if grep -q '^CXX_STD[[:space:]]*=' "$outfile"; then
			sed -E -i "s|^CXX_STD[[:space:]]*=.*|CXX_STD = ${std}|" "$outfile"
		else
			tmpfile="${outfile}.tmp"
			printf "CXX_STD = %s\n" "$std" > "$tmpfile"
			cat "$outfile" >> "$tmpfile"
			mv "$tmpfile" "$outfile"
		fi
	fi
done

echo "Prepared."

# Note: USE_CLANG should be set by the calling script (bench_loop.sh)
# and will be read by Makevars during R CMD INSTALL

# Embed cpp_std and cpp_compiler into central benchmark.R for reproducible output names
if [ -f ./scripts/run-benchmark.R ]; then
	# Replace the cpp_std assignment line with the full CXX token (e.g., CXX17)
	sed -E -i "s/^[[:space:]]*cpp_std[[:space:]]*<-[[:space:]]*\".*\"/cpp_std <- \"${std}\"/" ./scripts/run-benchmark.R || true
	# Replace the cpp_compiler assignment line
	sed -E -i "s/^[[:space:]]*cpp_compiler[[:space:]]*<-[[:space:]]*\".*\"/cpp_compiler <- \"${compiler}\"/" ./scripts/run-benchmark.R || true
fi
