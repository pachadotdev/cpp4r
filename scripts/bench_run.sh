#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX11}
compiler=${2:-gcc}

# Export the C++ standard and compiler so R can pick them up
export CPP_STD="$std"
export CPP_COMPILER="$compiler"

if [ -f "./run-benchmark.R" ]; then
	echo "Running central benchmark script: benchmarks/benchmark.R with std=$std"
	Rscript ./run-benchmark.R
fi

echo "Run complete."
