#!/usr/bin/env bash
set -euo pipefail

for compiler in gcc clang; do
  for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
    echo "==============================="
    echo "Benchmarking R code with $std standard and $compiler compiler"

    ./bench_prepare.sh "$std" "$compiler"
    ./bench_install.sh "$std"
    ./bench_run.sh "$std" "$compiler"
    ./bench_restore.sh "$std" "$compiler"

    echo "==============================="
    echo ""

  done
done
