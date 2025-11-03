#!/usr/bin/env bash
set -euo pipefail

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  echo "==============================="
  echo "Benchmarking R code with $std standard"

  ./scripts/bench_prepare.sh "$std"
  ./scripts/bench_install.sh
  ./scripts/bench_run.sh
  ./scripts/bench_restore.sh "$std"

  echo "==============================="
  echo ""

done
