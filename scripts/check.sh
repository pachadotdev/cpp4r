#!/usr/bin/env bash
# Docker-based R CMD check for cpp4r / cpp4rtest.
#
# Two modes, selected by whether a C++ standard is given as the 2nd arg:
#
#   check.sh <image>              Full CRAN-style check using the image's
#                                  default toolchain, e.g. "gcc16" for the
#                                  latest GCC, or "rocky8" / "debian10" for
#                                  other platforms.
#
#   check.sh <image> <std> [cc]   Also pins CXX to a single C++ standard
#                                  (cxx11|cxx14|cxx17|cxx20|cxx23) with
#                                  compiler "gcc" (default) or "clang", to
#                                  confirm clean compilation, e.g.
#                                  "check.sh ubuntu-release cxx17 clang".
#
# Usage: check.sh <rhub-image> [cxx-std] [gcc|clang]
set -euo pipefail

IMAGE="${1:?Usage: $0 <rhub-image> [cxx-std] [gcc|clang]}"
STD="${2:-}"
COMPILER="${3:-gcc}"

case "$STD" in
  "" ) STD_FLAG="" ;;
  cxx11) STD_FLAG="-std=c++11 -pedantic-errors -Wall -Wextra" ;;
  cxx14) STD_FLAG="-std=c++14 -pedantic-errors -Wall -Wextra" ;;
  cxx17) STD_FLAG="-std=c++17 -pedantic-errors -Wall -Wextra" ;;
  cxx20) STD_FLAG="-std=c++20 -pedantic-errors -Wall -Wextra" ;;
  cxx23) STD_FLAG="-std=c++23 -pedantic-errors -Wall -Wextra" ;;
  *) echo "Unknown C++ standard: $STD (expected cxx11|cxx14|cxx17|cxx20|cxx23)"; exit 1 ;;
esac

case "$COMPILER" in
  gcc|clang) ;;
  *) echo "Unknown compiler: $COMPILER (expected gcc|clang)"; exit 1 ;;
esac

if [ -n "${FULL_IMAGE_OVERRIDE:-}" ]; then
  FULL_IMAGE="$FULL_IMAGE_OVERRIDE"
else
  FULL_IMAGE="ghcr.io/r-hub/containers/${IMAGE}:latest"
fi

SUFFIX="${IMAGE}${STD:+-$STD-$COMPILER}"
LOG_DIR="./check-docker"
LOG="${LOG_DIR}/${SUFFIX}.log"
CACHE_DIR="$(pwd)/check-docker/cache/${SUFFIX}"
CHECK_DIR=$(mktemp -d)

mkdir -p "$LOG_DIR"
mkdir -p "$CACHE_DIR"
trap 'rm -rf "$CHECK_DIR"' EXIT

echo "==============================="
if [ -n "$STD" ]; then
  echo "Docker check: $IMAGE (forcing ${STD}, ${COMPILER})"
else
  echo "Docker check: $IMAGE (default toolchain)"
fi
echo "==============================="

if ! docker image inspect "$FULL_IMAGE" >/dev/null 2>&1; then
  echo "Pulling $FULL_IMAGE..."
  if ! docker pull "$FULL_IMAGE" >/dev/null 2>&1; then
    echo "Initial pull failed for $FULL_IMAGE"
    # If the image is the r-hub GHCR image and pull was denied, try common fallbacks
    FALLBACK=""
    if [[ "${FULL_IMAGE}" == ghcr.io/r-hub/containers/rocky8:* || "${IMAGE}" == "rocky8" ]]; then
      FALLBACK="docker.io/rockylinux/rockylinux:8"
    elif [[ "${IMAGE}" == "debian10" ]]; then
      FALLBACK="docker.io/library/debian:10-slim"
    else
      # Try a docker.io mirror of the same path if present
      FALLBACK="docker.io/${IMAGE}:latest"
    fi

    if [ -n "${FALLBACK}" ]; then
      echo "Attempting fallback image: ${FALLBACK}"
      if docker pull "${FALLBACK}" >/dev/null 2>&1; then
        echo "Using fallback image ${FALLBACK}"
        FULL_IMAGE="${FALLBACK}"
      else
        echo "Fallback pull failed for ${FALLBACK}; aborting."
        exit 1
      fi
    else
      echo "No fallback available; aborting."
      exit 1
    fi
  fi
else
  echo "Using cached image $FULL_IMAGE"
fi

echo "Building package tarballs..."

# devtools::build()/pkg_document() call roxygen2::roxygenise(), which for
# packages with compiled code triggers a full (often colorized, ANSI
# escape-laden) "R CMD INSTALL" *on the host*
# (using the host's own default compiler/CXX_STD, unrelated to the
# std/compiler this script is about to test inside Docker) to load the DLL.
# That output goes to stdout, so capturing the tarball path via
# `$(Rscript -e 'cat(...)')` directly would capture the noise too, producing
# a garbled path; it's also just confusing to see in the console, since it
# looks like (but isn't) the actual pinned-compiler check. Send it all to a
# dedicated build log instead, and have each Rscript call write its result to
# a temp file, then read the path back.
BUILD_LOG="${LOG_DIR}/${SUFFIX}-build.log"
: > "$BUILD_LOG"
CPP4R_TARBALL_FILE=$(mktemp)
CPP4RTEST_TARBALL_FILE=$(mktemp)
trap 'rm -rf "$CHECK_DIR" "$CPP4R_TARBALL_FILE" "$CPP4RTEST_TARBALL_FILE"' EXIT

# Re-vendor cpp4rtest/src/vendor/cpp4r from the current dev source before
# every check, so header-only edits (e.g. to inst/include/cpp4r/*.hpp) are
# actually exercised instead of silently checking whatever committed vendor
# copy happens to be sitting in cpp4rtest/src/vendor (which otherwise goes
# stale the moment a header changes without a manual re-vendor). vendor()
# copies from the *installed* cpp4r (via system.file), so plain
# `cpp4r::vendor()` would still copy stale bits if cpp4r itself hasn't been
# reinstalled; pkgload::load_all() shims system.file()/`::` to resolve to
# this live source tree instead, so vendor() picks up in-progress edits.
rm -rf ./cpp4rtest/src/vendor
Rscript -e "pkgload::load_all('.'); cpp4r::vendor('./cpp4rtest/src/vendor')" >>"$BUILD_LOG" 2>&1

Rscript -e 'cpp4r::register("./cpp4rtest")' >>"$BUILD_LOG" 2>&1
Rscript -e 'tinydev::pkg_document("./cpp4rtest")' >>"$BUILD_LOG" 2>&1
Rscript -e "writeLines(tinydev::pkg_build('.'), '${CPP4R_TARBALL_FILE}')" >>"$BUILD_LOG" 2>&1
Rscript -e "writeLines(tinydev::pkg_build('./cpp4rtest'), '${CPP4RTEST_TARBALL_FILE}')" >>"$BUILD_LOG" 2>&1

CPP4R_TARBALL=$(cat "$CPP4R_TARBALL_FILE")
CPP4RTEST_TARBALL=$(cat "$CPP4RTEST_TARBALL_FILE")

CPP4R_FILE=$(basename "$CPP4R_TARBALL")
CPP4RTEST_FILE=$(basename "$CPP4RTEST_TARBALL")

cp "$CPP4R_TARBALL" "$CHECK_DIR/"
cp "$CPP4RTEST_TARBALL" "$CHECK_DIR/"

# Create a minimal R script that installs the packages' own declared
# Imports/Suggests/LinkingTo (for a full test run, not just the CRAN-check
# metadata) when run inside the container. Dependencies are read straight
# out of each tarball's DESCRIPTION via base R's read.dcf (so this doesn't
# itself depend on any package being installed yet), rather than a
# hardcoded package list that silently goes stale whenever a test starts
# depending on a new Suggested package (e.g. 'desc').
cat > "$CHECK_DIR/install_required.R" <<'R_EOF'
user_lib <- strsplit(Sys.getenv('R_LIBS_USER'), ':')[[1]][1]
.libPaths(c(user_lib, .libPaths()))
options(repos = c('https://yihui.r-universe.dev', 'https://cloud.r-project.org'))

deps_from_tarball <- function(tarfile, own_names) {
  td <- tempfile()
  dir.create(td)
  utils::untar(tarfile, exdir = td)
  pkgdir <- list.dirs(td, recursive = FALSE)[1]
  dcf <- read.dcf(file.path(pkgdir, 'DESCRIPTION'))
  fields <- intersect(c('Depends', 'Imports', 'Suggests', 'LinkingTo'), colnames(dcf))
  if (length(fields) == 0) return(character())
  raw <- unlist(strsplit(dcf[1, fields], ','))
  raw <- trimws(sub('\\(.*\\)', '', raw))
  raw <- raw[nzchar(raw) & raw != 'R']
  setdiff(raw, own_names)
}

tarballs <- c(__TARBALLS__)
own_names <- c(__OWN_NAMES__)
pkgs <- unique(unlist(lapply(tarballs, deps_from_tarball, own_names = own_names)))
pkgs <- setdiff(pkgs, rownames(installed.packages()))

if (length(pkgs) > 0) {
  message('Installing declared dependencies: ', paste(pkgs, collapse = ', '))
  install.packages(pkgs, lib = user_lib)
}

R_EOF
sed -i \
  -e "s|__TARBALLS__|'/check/${CPP4R_FILE}', '/check/${CPP4RTEST_FILE}'|" \
  -e "s|__OWN_NAMES__|'cpp4r', 'cpp4rtest'|" \
  "$CHECK_DIR/install_required.R"

# When a C++ standard was requested, build the shell snippet that pins CXX to
# it inside the container. Left empty to use the image's default toolchain.
#
# Built from a fully-quoted heredoc (so nothing is expanded/escaped here) plus
# plain text substitutions, to keep the "evaluate later, inside the
# container" bits (bare $GXX/$GCC) free of backslash-escaping gymnastics.
MAKEVARS_STEP=""
if [ -n "$STD_FLAG" ]; then
  if [ "$COMPILER" = "clang" ]; then
    GXX_LINE='GXX="clang++"'
    GCC_LINE='GCC="clang"'
  else
    GXX_LINE='GXX=$(R CMD config CXX | awk '"'"'{print $1}'"'"')'
    GCC_LINE='GCC=$(R CMD config CC  | awk '"'"'{print $1}'"'"')'
  fi

  MAKEVARS_STEP=$(cat <<'HEREDOC'

      # R installations often bake the default standard into CXX itself
      # (e.g. CXX = g++ -std=gnu++20); `awk '{print $1}'` strips any such
      # flags, keeping just the bare compiler binary. The desired standard is
      # then applied exactly once via the CXX*STD variables, which R appends
      # to CXX* when invoking the compiler (baking it into CXX* too would
      # duplicate the flags on the command line).
      __GXX_LINE__
      __GCC_LINE__
      mkdir -p ~/.R
      {
        echo "CC=$GCC"
        echo "CXX=$GXX"
        echo "CXXSTD=__STD_FLAG__"
        echo "CXX11=$GXX"
        echo "CXX11STD=__STD_FLAG__"
        echo "CXX14=$GXX"
        echo "CXX14STD=__STD_FLAG__"
        echo "CXX17=$GXX"
        echo "CXX17STD=__STD_FLAG__"
        echo "CXX20=$GXX"
        echo "CXX20STD=__STD_FLAG__"
        echo "CXX23=$GXX"
        echo "CXX23STD=__STD_FLAG__"
      } > ~/.R/Makevars
HEREDOC
)
  MAKEVARS_STEP="${MAKEVARS_STEP//__GXX_LINE__/$GXX_LINE}"
  MAKEVARS_STEP="${MAKEVARS_STEP//__GCC_LINE__/$GCC_LINE}"
  MAKEVARS_STEP="${MAKEVARS_STEP//__STD_FLAG__/$STD_FLAG}"
fi

# Extra system packages to install so the requested compiler is available.
EXTRA_APT_PKGS=""
EXTRA_DNF_PKGS=""
EXTRA_ZYPPER_PKGS=""
if [ "$COMPILER" = "clang" ]; then
  # clang needs LLVM's OpenMP runtime (libomp) explicitly: unlike gcc, whose
  # -fopenmp support (libgomp) ships with the gcc package itself, clang's
  # -fopenmp links against -lomp, which isn't installed by the "clang"
  # package alone.
  EXTRA_APT_PKGS="clang libomp-dev"
  EXTRA_DNF_PKGS="clang libomp-devel"
  EXTRA_ZYPPER_PKGS="clang libomp-devel"
fi

# cpp4rtest/configure regenerates src/Makevars from Makevars.in, honoring
# this env var for CXX_STD (defaulting to CXX23 when unset/empty).
CPP4RTEST_CXX_STD=""
if [ -n "$STD" ]; then
  CPP4RTEST_CXX_STD=$(echo "$STD" | tr '[:lower:]' '[:upper:]')
fi

clear

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
    # Debian/Ubuntu's r-base Makeconf honors DEB_BUILD_OPTIONS=noopt by
    # rebuilding CFLAGS/CXXFLAGS as '-UNDEBUG -Wall -pedantic -g -O0',
    # overriding the normal -O2. That -O0 then conflicts with the image's
    # hardened -D_FORTIFY_SOURCE=3 default (glibc requires -O1+ for it to
    # take effect), producing a '#warning' on every compile. Clearing it
    # restores normal -O2 optimized builds inside the container.
    unset DEB_BUILD_OPTIONS
    # litedown (and possibly other dev-only deps) isn't on CRAN yet, so any
    # automatic install of missing dependencies inside this container (e.g.
    # R CMD check's own install of missing Suggests, or the ad-hoc 'curl'
    # install below) needs a fallback repo. Writing to Rprofile.site rather
    # than a single script's options(repos = ...) makes the setting global:
    # it's sourced by every R/Rscript process started in this container from
    # here on, not just the one process that calls options() itself.
    RHOME=\$(R RHOME)
    mkdir -p \"\$RHOME/etc\"
    cat >> \"\$RHOME/etc/Rprofile.site\" <<'RPROFILE_EOF'
options(repos = c('https://yihui.r-universe.dev', 'https://cloud.r-project.org'))
RPROFILE_EOF
    mkdir -p /cache/R_libs
    # Install minimal system build deps needed by R packages (libuv for 'fs')
    if command -v apt-get >/dev/null 2>&1; then
      export DEBIAN_FRONTEND=noninteractive
      apt-get update -qq || true
      apt-get install -y --no-install-recommends \
        devscripts pkg-config gfortran libcurl4-openssl-dev ${EXTRA_APT_PKGS} || true
    elif command -v dnf >/dev/null 2>&1 || command -v yum >/dev/null 2>&1; then
      PKG_MGR=\$(command -v dnf 2>/dev/null || echo yum)
      \$PKG_MGR -y install pkgconfig gcc-gfortran libcurl-devel ${EXTRA_DNF_PKGS} || true
    elif command -v zypper >/dev/null 2>&1; then
      zypper --non-interactive install pkg-config gcc-fortran libcurl-devel ${EXTRA_ZYPPER_PKGS} || true
    fi

    # Install system deps (xml2, etc) and R package deps before writing
    # ~/.R/Makevars so packages with C++ code (diffobj, etc) compile with the
    # image's default standard instead of the one under test.
    if [ -f /check/install_required.R ]; then Rscript /check/install_required.R || true; fi
    
    # --as-cran's 'checking CRAN incoming feasibility' step uses the 'curl'
    # R package to verify URLs/DOIs in the docs. It isn't a dependency of
    # any of our packages; without it, URL/DOI verification errors out
    # (rather than just flagging a bad link), which escalates that check from an
    # informational NOTE to a WARNING.
    Rscript -e \"if (!requireNamespace('curl', quietly = TRUE)) install.packages('curl', lib = Sys.getenv('R_LIBS_USER'))\" || true
${MAKEVARS_STEP}
    # Remove stale locks and old cpp4r/cpp4rtest before reinstalling
    rm -rf /cache/R_libs/00LOCK-* /cache/R_libs/cpp4r /cache/R_libs/cpp4rtest
    export CPP4RTEST_CXX_STD='${CPP4RTEST_CXX_STD}'
    R CMD INSTALL --library=/cache/R_libs /check/${CPP4R_FILE}
    R CMD INSTALL --library=/cache/R_libs /check/${CPP4RTEST_FILE}
    cd /check
    export _R_CHECK_FORCE_SUGGESTS_=false
    R CMD check --as-cran --no-manual ${CPP4RTEST_FILE}
  " 2>&1 | grep -v 'readelf: Warning:' | tee "${CHECK_DIR}/docker.log" || DOCKER_RC="${PIPESTATUS[0]}"

cp "${CHECK_DIR}/docker.log" "$LOG"

if [ -d "${CHECK_DIR}/cpp4rtest.Rcheck" ]; then
  RCHECK_DEST="${LOG_DIR}/${SUFFIX}-cpp4rtest.Rcheck"
  rm -rf "$RCHECK_DEST"
  cp -r "${CHECK_DIR}/cpp4rtest.Rcheck" "$RCHECK_DEST"
  rm "$BUILD_LOG"
  echo "Rcheck directory saved to: ${RCHECK_DEST}"
fi

echo "==============================="
echo "Check complete. Log: $LOG"
echo "==============================="

exit $DOCKER_RC
