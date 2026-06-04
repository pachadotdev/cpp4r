#!/usr/bin/env bash
# Fast CXX-standard check using a single local Docker image.
# Unlike check-cxx.sh, this does NOT install per-standard GCC versions from a
# PPA — it uses whatever compiler the image ships with and only varies the
# -std=gnu++XX flag.  This makes each run much faster and avoids network hangs.
set -euo pipefail

IMAGE="${1:-ubuntu-release}"
FULL_IMAGE="ghcr.io/r-hub/containers/${IMAGE}:latest"

LOG_DIR="./check-docker"
CACHE_DIR="$(pwd)/check-docker/cache/${IMAGE}-cxx"
STANDARDS="CXX11 CXX14 CXX17 CXX20 CXX23"
PASS=()
FAIL=()

mkdir -p "$LOG_DIR"
mkdir -p "$CACHE_DIR"

echo "==============================="
echo "Local CXX standard check: $IMAGE"
echo "==============================="

# Pull image once
if ! docker image inspect "$FULL_IMAGE" >/dev/null 2>&1; then
  echo "Pulling $FULL_IMAGE..."
  if ! docker pull "$FULL_IMAGE" >/dev/null 2>&1; then
    echo "Failed to pull $FULL_IMAGE; aborting."
    exit 1
  fi
else
  echo "Using cached image $FULL_IMAGE"
fi

# Build tarballs once on the host; the standard is forced inside each container
# via ~/.R/Makevars, so the tarballs are the same for every run.
echo "Building package tarballs..."
Rscript -e 'cpp4r::register("./cpp4rtest")'
Rscript -e 'devtools::document("./cpp4rtest")'
CPP4R_TARBALL=$(Rscript -e 'cat(devtools::build(".", quiet = TRUE))')
CPP4RTEST_TARBALL=$(Rscript -e 'cat(devtools::build("./cpp4rtest", quiet = TRUE))')
CPP4R_FILE=$(basename "$CPP4R_TARBALL")
CPP4RTEST_FILE=$(basename "$CPP4RTEST_TARBALL")

# CRAN does not like #pragma GCC diagnostic ignored "-Wcast-function-type"
for std in $STANDARDS; do
  VER="${std#CXX}"  # 11, 14, 17, 20, 23
  case "$VER" in
    11) STD_FLAG="-std=c++11 -pedantic-errors -Wall -Wextra" ;;
    14) STD_FLAG="-std=c++14 -pedantic-errors -Wall -Wextra" ;;
    17) STD_FLAG="-std=c++17 -pedantic-errors -Wall -Wextra" ;;
    20) STD_FLAG="-std=c++20 -pedantic-errors -Wall -Wextra" ;;
    23) STD_FLAG="-std=c++23 -pedantic-errors -Wall -Wextra" ;;
    *)  echo "Unknown standard: $std"; continue ;;
  esac

  LOG="${LOG_DIR}/cxx${VER}.log"
  CHECK_DIR=$(mktemp -d)

  echo ""
  echo "==============================="
  echo "Checking ${std} (${STD_FLAG})"
  echo "==============================="
  echo ""

  cp "$CPP4R_TARBALL" "$CHECK_DIR/"
  cp "$CPP4RTEST_TARBALL" "$CHECK_DIR/"

  cat > "$CHECK_DIR/install_required.R" <<'R_EOF'
user_lib <- strsplit(Sys.getenv('R_LIBS_USER'), ':')[[1]][1]
.libPaths(c(user_lib, .libPaths()))
repos_snapshot_env <- Sys.getenv('RSPM_SNAPSHOT', '')

if (nzchar(repos_snapshot_env)) {
  if (grepl('^https?://', repos_snapshot_env)) {
    options(repos = c(CRAN = repos_snapshot_env))
  } else {
    options(repos = c(CRAN = paste0('https://packagemanager.rstudio.com/cran/', repos_snapshot_env)))
  }
} else {
  options(repos = c(CRAN = 'https://cloud.r-project.org'))
}

if (!requireNamespace('remotes', quietly = TRUE)) {
  install.packages('remotes', lib = user_lib)
}

install.packages('tinytest', lib = user_lib)

if (!requireNamespace('curl', quietly = TRUE)) {
  install.packages('curl', lib = user_lib)
}

if (!requireNamespace('xml2', quietly = TRUE)) {
  install.packages('xml2', lib = user_lib)
}

if (!requireNamespace('rlang', quietly = TRUE)) {
  install.packages('rlang', lib = user_lib)
}
R_EOF

  DOCKER_RC=0
  docker run --rm \
    -v "${CHECK_DIR}:/check" \
    -v "${CACHE_DIR}:/cache" \
    "$FULL_IMAGE" \
    bash -c "
      set -euo pipefail
      show_logs_and_fix_perms() {
        echo '=== 00install.out ==='
        cat /check/cpp4rtest.Rcheck/00install.out || true
        echo '=== 00check.log ==='
        cat /check/cpp4rtest.Rcheck/00check.log || true
        chmod -R a+rwX /check
      }
      trap show_logs_and_fix_perms EXIT

      export R_LIBS_USER=/cache/R_libs
      export R_LIBS=/cache/R_libs
      mkdir -p /cache/R_libs

      # Install only the minimal system deps; no PPA, no version-specific GCC.
      if command -v apt-get >/dev/null 2>&1; then
        export DEBIAN_FRONTEND=noninteractive
        apt-get update -qq || true
        apt-get install -y --no-install-recommends \
          libuv1-dev libxml2-dev pkg-config gfortran libcurl4-openssl-dev || true
      elif command -v dnf >/dev/null 2>&1 || command -v yum >/dev/null 2>&1; then
        PKG_MGR=\$(command -v dnf 2>/dev/null || echo yum)
        \$PKG_MGR -y install libuv-devel libxml2-devel pkgconfig gcc-gfortran libcurl-devel || true
      elif command -v zypper >/dev/null 2>&1; then
        zypper --non-interactive install libuv libxml2-devel pkg-config gcc-fortran libcurl-devel || true
      fi

      # Install system deps (testthat, xml2, …) before writing ~/.R/Makevars so
      # that packages with C++ code (diffobj, …) compile with the image's default
      # standard instead of the one under test.
      if [ -f /check/install_required.R ]; then Rscript /check/install_required.R || true; fi

      # R installations often bake the default standard into CXX itself
      # (e.g. CXX = g++ -std=gnu++20), so overriding only CXX*STD has no effect.
      # Extract the bare compiler binary and re-embed the desired standard into
      # every CXX* variable so the override is guaranteed to take effect.
      GXX=\$(R CMD config CXX | awk '{print \$1}')
      GCC=\$(R CMD config CC  | awk '{print \$1}')

      mkdir -p ~/.R
      {
        echo \"CC=\$GCC\"
        echo \"CXX=\$GXX ${STD_FLAG}\"
        echo \"CXXSTD=${STD_FLAG}\"
        echo \"CXX11=\$GXX ${STD_FLAG}\"
        echo \"CXX11STD=${STD_FLAG}\"
        echo \"CXX14=\$GXX ${STD_FLAG}\"
        echo \"CXX14STD=${STD_FLAG}\"
        echo \"CXX17=\$GXX ${STD_FLAG}\"
        echo \"CXX17STD=${STD_FLAG}\"
        echo \"CXX20=\$GXX ${STD_FLAG}\"
        echo \"CXX20STD=${STD_FLAG}\"
        echo \"CXX23=\$GXX ${STD_FLAG}\"
        echo \"CXX23STD=${STD_FLAG}\"
      } > ~/.R/Makevars

      rm -rf /cache/R_libs/00LOCK-* /cache/R_libs/cpp4r /cache/R_libs/cpp4rtest
      R CMD INSTALL --library=/cache/R_libs /check/${CPP4R_FILE}
      R CMD INSTALL --no-staged-install --library=/cache/R_libs /check/${CPP4RTEST_FILE}

      cd /check
      export _R_CHECK_FORCE_SUGGESTS_=false
      R CMD check --as-cran --no-manual ${CPP4RTEST_FILE}
    " 2>&1 | grep -v 'readelf: Warning:' | tee "${CHECK_DIR}/docker.log" || DOCKER_RC="${PIPESTATUS[0]}"

  cp "${CHECK_DIR}/docker.log" "$LOG"

  if [ -d "${CHECK_DIR}/cpp4rtest.Rcheck" ]; then
    RCHECK_DEST="${LOG_DIR}/cxx${VER}-cpp4rtest.Rcheck"
    rm -rf "$RCHECK_DEST"
    cp -r "${CHECK_DIR}/cpp4rtest.Rcheck" "$RCHECK_DEST"
    echo "Rcheck directory saved to: ${RCHECK_DEST}"
  fi

  if [ "$DOCKER_RC" -eq 0 ]; then
    PASS+=("$std")
  else
    FAIL+=("$std")
  fi

  rm -rf "$CHECK_DIR"

  echo ""
  echo "==============================="
  echo "Done ${std}. Log: ${LOG}"
  echo "==============================="
done

echo ""
echo "==============================="
echo "Results"
echo "==============================="
printf "PASS: %s\n" "${PASS[*]:-none}"
printf "FAIL: %s\n" "${FAIL[*]:-none}"
echo ""
[ "${#FAIL[@]}" -eq 0 ]
