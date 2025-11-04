#!/usr/bin/env bash
set -euo pipefail

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  for compiler in clang gcc; do
    echo "==============================="
    echo "Benchmarking R code with $std standard and $compiler compiler"

    # Set USE_CLANG environment variable for this iteration
    if [ "$compiler" = "clang" ]; then
      export USE_CLANG=1
    else
      unset USE_CLANG || true
    fi

    ./bench_prepare.sh "$std" "$compiler"
    ./bench_install.sh "$std"
    ./bench_run.sh "$std" "$compiler"
    ./bench_restore.sh "$std" "$compiler"

    echo "==============================="
    echo ""

  done
done
