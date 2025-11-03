#!/usr/bin/env bash
set -euo pipefail

for std in CXX11 CXX14 CXX17 CXX20 CXX23; do
  echo " "
  echo "==============================="
  echo "Testing C++ code with $std standard"
  echo "CXX_STD = $std" > ./cpp4rtest/src/Makevars.in

  if [ "$std" = "CXX11" ]; then cpp_std="C++11"
  elif [ "$std" = "CXX14" ]; then cpp_std="C++14"
  elif [ "$std" = "CXX17" ]; then cpp_std="C++17"
  elif [ "$std" = "CXX20" ]; then cpp_std="C++20"
  elif [ "$std" = "CXX23" ]; then cpp_std="C++23"
  else cpp_std="$std"; fi

  sed -i "s/C++NN/${cpp_std}/" ./cpp4rtest/DESCRIPTION
  echo "CXX_STD = $std" > ./cpp4rtest/src/Makevars.in

  Rscript -e 'devtools::clean_dll("cpp4rtest"); cpp4r::register("cpp4rtest"); devtools::document("cpp4rtest");'
  Rscript -e 'devtools::load_all("cpp4rtest"); devtools::test("cpp4rtest")'

  sed -i "s/${std}/C++NN/" ./cpp4rtest/DESCRIPTION || true
  echo "" > ./cpp4rtest/src/Makevars.in

  echo "==============================="
done
