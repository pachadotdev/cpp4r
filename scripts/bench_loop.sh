#!/usr/bin/env bash
set -euo pipefail

# for std in CXX23 CXX20 CXX17 CXX14 CXX11; do
for std in CXX23; do
  for compiler in clang gcc; do
    echo "==============================="
    echo "Benchmarking R code with $std standard and $compiler compiler"

    # if results/bench_results_%s_%s.rds exists, skip this iteration
    if [ -f "results/bench_results_${std}_${compiler}.rds" ]; then
      echo "Results for std=$std compiler=$compiler already exist; skipping this iteration."
      echo "==============================="
      echo ""
      continue
    fi

    # Set USE_CLANG environment variable for this iteration
    if [ "$compiler" = "clang" ]; then
      export USE_CLANG=1
    else
      unset USE_CLANG || true
    fi

    # Run steps with explicit handling so we can treat install failures as fatal
    # (restore and exit), while prepare/run failures restore and continue.

    # Prepare
    if ! ./scripts/bench_prepare.sh "$std" "$compiler"; then
      rc=$?
      echo "Prepare failed (std=$std compiler=$compiler) with code $rc; running restore and exiting benchmark run..."
      ./scripts/bench_restore.sh "$std" "$compiler" || true
      exit $rc
    fi

    # Install (fatal on failure)
    if ! ./scripts/bench_install.sh "$std"; then
      rc=$?
      echo "Install failed (std=$std compiler=$compiler) with code $rc; running restore and exiting benchmark run..."
      ./scripts/bench_restore.sh "$std" "$compiler" || true
      exit $rc
    fi

    # Run
    if ! ./scripts/bench_run.sh "$std" "$compiler"; then
      rc=$?
      echo "Run failed (std=$std compiler=$compiler) with code $rc; running restore and exiting benchmark run..."
      ./scripts/bench_restore.sh "$std" "$compiler" || true
      exit $rc
    fi

    # Success: restore to neutral state
    ./scripts/bench_restore.sh "$std" "$compiler" || true

    echo "==============================="
    echo ""
  done
done
