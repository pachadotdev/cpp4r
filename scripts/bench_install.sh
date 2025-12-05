#!/usr/bin/env bash
set -euo pipefail

# Accept arguments but ignore them (for Makefile compatibility)
# std=$1
# compiler=$2

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
    
    # Register functions for packages that need it
    if [ "$pkg" = "cpp4rbenchmark" ]; then
      echo "Registering cpp4r functions for $pkg"
      Rscript -e "cpp4r::register('./extended-tests/$pkg')" || {
        echo "ERROR: Failed to register cpp4r functions for $pkg"
        exit 1
      }
    elif [ "$pkg" = "cpp11benchmark" ]; then
      echo "Registering cpp11 functions for $pkg"
      Rscript -e "cpp11::cpp_register('./extended-tests/$pkg')" || {
        echo "ERROR: Failed to register cpp11 functions for $pkg"
        exit 1
      }
    elif [ "$pkg" = "Rcppbenchmark" ]; then
      echo "Compiling Rcpp attributes for $pkg"
      Rscript -e "Rcpp::compileAttributes('./extended-tests/$pkg')" || {
        echo "ERROR: Failed to compile Rcpp attributes for $pkg"
        exit 1
      }
    fi
    
    # Document
    # intentional double document to avoid
    # Warning: Objects listed as exports, but not present in namespace:
    Rscript -e "devtools::document('./extended-tests/$pkg'); devtools::document('./extended-tests/$pkg')" || echo "Warning: Documentation failed for $pkg"
    
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
