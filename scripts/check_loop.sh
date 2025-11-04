#!/usr/bin/env bash
set -euo pipefail

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  for compiler in gcc clang; do
    echo "==============================="
    echo "Checking C++ code with $std standard and $compiler compiler"

    ./scripts/check_prepare.sh "$std" "$compiler"
    
    # Run check, but don't exit on failure
    if ! ./scripts/check_run.sh; then
      echo "WARNING: check_run.sh failed for $std standard with $compiler, continuing..."
    fi

    ./scripts/check_restore.sh "$std" "$compiler"

    echo "==============================="
    echo ""

  done
done
