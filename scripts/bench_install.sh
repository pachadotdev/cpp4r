#!/usr/bin/env bash
set -euo pipefail

echo "Cleaning, documenting and installing benchmark packages"

# Ensure USE_CLANG is exported if it's set
if [ -n "${USE_CLANG:-}" ]; then
  export USE_CLANG
  echo "USE_CLANG is set: $USE_CLANG (will use Clang)"
else
  echo "USE_CLANG is not set (will use GCC)"
fi

# Install each package individually with error handling
pkgs=("cpp4rbenchmark" "cpp11benchmark" "Rcppbenchmark")

for pkg in "${pkgs[@]}"; do
  if [ -d "./extended-tests/$pkg" ]; then
    echo "==============================="
    echo "Processing: $pkg"
    echo "==============================="
    
    # Clean DLL
    Rscript -e "devtools::clean_dll('./extended-tests/$pkg')" || true
    
    # Document
    Rscript -e "devtools::document('./extended-tests/$pkg')" || echo "Warning: Documentation failed for $pkg"
    
    # Install
    Rscript -e "devtools::install('./extended-tests/$pkg', upgrade = 'never')" || {
      echo "ERROR: Failed to install $pkg"
      exit 1
    }
    
    echo ""
  else
    echo "Warning: Package directory ./extended-tests/$pkg not found, skipping"
  fi
done

echo "Install complete."
