clean:
	@clear
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp4rtest");'
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp11benchmark")'
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp4rbenchmark")'
	@Rscript -e 'devtools::clean_dll("extended-tests/Rcppbenchmark")'

install:
	@clear
	@Rscript -e 'devtools::install("./")'

docs:
	@clear
	@Rscript -e 'devtools::document("./"); pkgsite::build_site("./")'

check:
	@clear
	@echo "==============================="
	@echo "Checking R code"
	@$(MAKE) install
	@Rscript -e	'cpp4r::register("extended-tests/cpp4rtest")'
	@Rscript -e 'devtools::check("./", error_on = "error")'
	@echo "==============================="
	@echo "Checking C++ code"
	@$(MAKE) install
	@rm -f extended-tests-results/*.log
	@rm -f extended-tests-results/check-results.md
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/check_loop.sh'
	@echo "==============================="

bench:
	@clear
	@rm -f extended-tests-results/*.rds
	@rm -f extended-tests-results/bench_summary.md
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/bench_loop.sh'
	@Rscript './scripts/combine-benchmarks.R'

clang_format=`which clang-format-18`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cpp')
	@${clang_format} -i $?
