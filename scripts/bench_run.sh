#!/usr/bin/env bash
set -euo pipefail

# Run the bench script (will exit on error)
Rscript './cpp4rbenchmark/bench/run.R'

echo "Run complete."
