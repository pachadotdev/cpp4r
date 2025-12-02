#!/usr/bin/env bash
set -euo pipefail

# Accept std and compiler as positional parameters
std="$1"
std=$(echo "$std" | tr '[:lower:]' '[:upper:]')
compiler="$2"

# Export USE_CLANG for R subprocesses if it's set
if [ -n "${USE_CLANG:-}" ]; then
  export USE_CLANG
fi

# Export CXX_STD for configure script
export CXX_STD="${std}"

# Ensure results directory and set per-iteration log
mkdir -p ./extended-tests-results
LOG="./extended-tests-results/check-${std}-${compiler}.log"

# clear previous log if it exists
rm -f "${LOG}"

# Capture everything (stdout+stderr) from this point into the per-iteration log
# while still printing to the console via tee. This ensures all printed lines
# (from Rscript, R CMD check and this script) are saved.
exec > >(tee -a "${LOG}") 2>&1

# Run the bench script (will exit on error)
Rscript -e 'cpp4r::register("./extended-tests/cpp4rtest")'
Rscript -e 'devtools::document("./extended-tests/cpp4rtest")'

# Build package tarball first (devtools::build returns path)
TARBALL=$(Rscript -e 'cat(devtools::build("./extended-tests/cpp4rtest", quiet = TRUE))')
if [ -z "${TARBALL}" ]; then
	echo "Failed to build tarball for cpp4rtest."
	exit 1
fi

# Run R CMD check on the tarball and capture output. Skip PDF/manual to avoid TeX font issues.
CXX_STD="${std}" R CMD check --as-cran --no-manual "${TARBALL}" || true

# If there was an error, copy the install log to the results directory for inspection
if [ -f "./cpp4rtest.Rcheck/00install.out" ]; then
	cp "./cpp4rtest.Rcheck/00install.out" "./extended-tests-results/install-${std}-${compiler}.log"
	echo "=== BEGIN 00install.out ==="
	cat "./cpp4rtest.Rcheck/00install.out"
	echo "=== END 00install.out ==="
fi

# Inspect log for ERRORs only. Allow WARNINGs and NOTEs.
if grep -q "\bERROR\b" "${LOG}"; then
	echo "R CMD check found ERRORs. See ${LOG} for details."
	# Print a short excerpt for convenience
	grep -n "\bERROR\b" -n "${LOG}" || true
	exit 1
else
	echo "R CMD check completed with no ERRORs. Warnings/Notes (if any) are allowed. See ${LOG} for full output."
fi

rm -f "${TARBALL}"

echo "Run complete."
