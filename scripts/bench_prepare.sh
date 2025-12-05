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

export CPP_STD="${std}"
export CPP_COMPILER="${compiler}"

echo "Prepared."
