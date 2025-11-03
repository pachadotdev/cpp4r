#!/usr/bin/env bash
set -euo pipefail

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  echo "==============================="
  echo "Benchmarking R code with $std standard"

  ./scripts/check_prepare.sh "$std"
  
  # Run check, but don't exit on failure
  if ! ./scripts/check_run.sh; then
    echo "WARNING: check_run.sh failed for $std standard, continuing..."
  fi

  ./scripts/check_restore.sh "$std"

  echo "==============================="
  echo ""

done
