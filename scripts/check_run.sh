#!/usr/bin/env bash
set -euo pipefail

# Accept std and compiler as positional parameters
std="$1"
compiler="$2"

# Export USE_CLANG for R subprocesses if it's set
if [ -n "${USE_CLANG:-}" ]; then
  export USE_CLANG
fi

# Ensure results directory and set per-iteration log
mkdir -p ./extended-tests-results
LOG="./extended-tests-results/check-${std}-${compiler}.log"

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
R CMD check --as-cran --no-manual "${TARBALL}" || true

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
