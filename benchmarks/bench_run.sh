#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}

# Export the C++ standard so R can pick it up via Sys.getenv("CPP_STD")
export CPP_STD="$std"

if [ -f "./benchmark.R" ]; then
	echo "Running central benchmark script: benchmarks/benchmark.R with std=$std"
	Rscript ./benchmark.R
else
	echo "No central benchmark.R found in current directory; attempting package run.R files"
	pkgs=(cpp4rbenchmark cpp11benchmark Rcppbenchmark)
	for pkg in "${pkgs[@]}"; do
		if [ -f "./${pkg}/bench/run.R" ]; then
			echo "Running ${pkg}/bench/run.R"
			Rscript "./${pkg}/bench/run.R"
		else
			echo "No run.R for ${pkg}, skipping"
		fi
	done
fi

echo "Run complete."
