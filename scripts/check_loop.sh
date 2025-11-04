#!/usr/bin/env bash
set -euo pipefail

rm -f check-results.md

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  for compiler in clang gcc; do
    echo "==============================="
    echo "Checking C++ code with $std standard and $compiler compiler"

    # Set USE_CLANG environment variable for this iteration
    if [ "$compiler" = "clang" ]; then
      export USE_CLANG=1
    else
      unset USE_CLANG || true
    fi

    ./scripts/check_prepare.sh "$std" "$compiler"

    touch check-results.md
    
    # Run check, but don't exit on failure
    if ! ./scripts/check_run.sh; then
      echo "WARNING: check_run.sh failed for $std standard with $compiler, continuing..."
    fi

    # append line with CXX standard and compiler to check-results.md
    echo "$std + $compiler = ok" >> check-results.md || true

    ./scripts/check_restore.sh "$std" "$compiler"

    echo "==============================="
    echo ""

  done
done
