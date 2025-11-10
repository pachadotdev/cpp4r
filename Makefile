clean:
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp4rtest"); cpp4r::register("extended-tests/cpp4rtest")'
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp11benchmark")'
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp4rbenchmark")'
	@Rscript -e 'devtools::clean_dll("extended-tests/Rcppbenchmark")'

install:
	@Rscript -e 'devtools::clean_dll("cpp4rtest"); devtools::install()'

docs:
	@Rscript -e 'devtools::document(); pkgsite::build_site()'

test:
	@clear
	@echo "==============================="
	@echo "Testing R code"
	@Rscript -e 'devtools::document(); devtools::test(); devtools::install()'
	@echo "==============================="
	@/bin/bash -euo pipefail -c './extended-tests/test_loop.sh'

check:
	@clear
	@echo "==============================="
	@echo "Checking R code"
	@Rscript -e 'devtools::install(); devtools::check(error_on = "error")'
	@clear
	@echo "==============================="
	@echo "Checking C++ code"
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './extended-tests/check_loop.sh'
	@echo "==============================="

bench:
	@clear
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './bench_loop.sh'

clang_format=`which clang-format-18`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cpp')
	@${clang_format} -i $?
