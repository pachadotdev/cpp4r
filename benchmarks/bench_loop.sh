#!/usr/bin/env bash
set -euo pipefail

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  echo "==============================="
  echo "Benchmarking R code with $std standard"

  ./bench_prepare.sh "$std"
  ./bench_install.sh "$std"
  ./bench_run.sh "$std"
  ./bench_restore.sh "$std"

  echo "==============================="
  echo ""

done
