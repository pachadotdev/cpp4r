#!/usr/bin/env bash
set -euo pipefail

# Run the bench script (will exit on error)
Rscript -e 'cpp4r::register("cpp4rtest")'
Rscript -e 'devtools::document("cpp4rtest")'
PACKAGE="cpp4rtest"
LOG="${PACKAGE}-check.log"

# Build package tarball first (devtools::build returns path)
TARBALL=$(Rscript -e 'cat(devtools::build("cpp4rtest", quiet = TRUE))' 2>/dev/null)
if [ -z "${TARBALL}" ]; then
	echo "Failed to build tarball for ${PACKAGE}."
	exit 1
fi

# Run R CMD check on the tarball and capture output. Skip PDF/manual to avoid TeX font issues.
R CMD check --as-cran --no-manual "${TARBALL}" &> "${LOG}" || true

# Inspect log for ERRORs only. Allow WARNINGs and NOTEs.
if grep -q "\bERROR\b" "${LOG}"; then
	echo "R CMD check found ERRORs. See ${LOG} for details."
	# Print a short excerpt for convenience
	grep -n "\bERROR\b" -n "${LOG}" || true
	exit 1
else
	echo "R CMD check completed with no ERRORs. Warnings/Notes (if any) are allowed. See ${LOG} for full output."
fi

echo "Run complete."
