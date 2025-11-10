#!/usr/bin/env bash
set -euo pipefail

# Accept std as first arg, default to CXX11
std=${1:-CXX11}
compiler=${2:-gcc}

echo "Restoring files for $std and $compiler"

# Convert std to C++ format for DESCRIPTION (e.g., CXX20 -> C++20)
if [ "$std" = "CXX11" ]; then cpp_std="C++11"
elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
else cpp_std="$std"; fi

mfile="./extended-tests/cpp4rtest/src/Makevars.in"
if [ -f "$mfile" ]; then
    # replace existing CXX_STD line (handles optional quotes)
    if grep -q '^CXX_STD[[:space:]]*=' "$mfile"; then
        sed -E -i 's|^CXX_STD[[:space:]]*=.*|CXX_STD = CXXNN|' "$mfile"
    else
        # Prepend CXX_STD line to the existing Makevars.in
        tmpfile="${mfile}.tmp"
        printf "CXX_STD = CXXNN\n" > "$tmpfile"
        cat "$mfile" >> "$tmpfile"
        mv "$tmpfile" "$mfile"
    fi
else
    # File doesn't exist: create with CXX_STD line
    echo "CXX_STD = CXXNN" > "$mfile"
fi

# Restore GCC by unsetting USE_CLANG
unset USE_CLANG || true

# Clear check files
rm -rf ./cpp4rtest.Rcheck || true
