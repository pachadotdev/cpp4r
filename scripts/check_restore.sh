#!/usr/bin/env bash
set -euo pipefail

std=${1:-CXX17}
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
compiler=${2:-gcc}

echo "Restoring files for $std and $compiler"

# Restore CXX_STD to cpp4rtest's default (CXX23)
(cd ./cpp4rtest && ./configure)

# Clear check files
rm -rf ./cpp4rtest.Rcheck || true
