#!/usr/bin/env bash
set -euo pipefail

FULL_IMAGE="ghcr.io/r-hub/containers/rchk:latest"
LOG_DIR="./check-docker"
LOG="${LOG_DIR}/rchk.log"
CHECK_DIR=$(mktemp -d)

mkdir -p "$LOG_DIR"
trap 'rm -rf "$CHECK_DIR"' EXIT

echo "==============================="
echo "rchk Docker check"
echo "==============================="

docker pull "$FULL_IMAGE"

echo "Building package tarballs..."
Rscript -e 'cpp4r::register("./cpp4rtest")'
Rscript -e 'devtools::document("./cpp4rtest")'

CPP4R_TARBALL=$(Rscript -e 'cat(devtools::build(".", quiet = TRUE))')
CPP4RTEST_TARBALL=$(Rscript -e 'cat(devtools::build("./cpp4rtest", quiet = TRUE))')

CPP4R_FILE=$(basename "$CPP4R_TARBALL")
CPP4RTEST_FILE=$(basename "$CPP4RTEST_TARBALL")

cp "$CPP4R_TARBALL" "$CHECK_DIR/"
cp "$CPP4RTEST_TARBALL" "$CHECK_DIR/"

rm -f "$LOG"

# Install cpp4r so rchk can find it as a LinkingTo dependency, then remove it
# from /check so that r-check only analyses cpp4rtest.
docker run --rm \
  -v "${CHECK_DIR}:/check" \
  "$FULL_IMAGE" \
  bash -c "
    set -euo pipefail
    R CMD INSTALL /check/${CPP4R_FILE}
    rm -f /check/${CPP4R_FILE}
    r-check
  " 2>&1 | tee "$LOG"

echo "==============================="
echo "rchk complete. Log: $LOG"
echo "==============================="
