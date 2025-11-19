clean:
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp4rtest");'
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp11benchmark")'
	@Rscript -e 'devtools::clean_dll("extended-tests/cpp4rbenchmark")'
	@Rscript -e 'devtools::clean_dll("extended-tests/Rcppbenchmark")'

register:
	@Rscript -e	'cpp4r::register("extended-tests/cpp4rtest")'
	@Rscript -e	'cpp4r::register("extended-tests/cpp4rbenchmark")'

install:
	@Rscript -e 'devtools::install("./")'

docs:
	@Rscript -e 'devtools::document("./"); pkgsite::build_site("./")'

check:
	@echo "==============================="
	@echo "Checking R code"
	@$(MAKE) clean
	@$(MAKE) install
	@$(MAKE) register
	@Rscript -e	'cpp4r::register("extended-tests/cpp4rtest")'
	@Rscript -e 'devtools::check("./", error_on = "error")'
	@echo "==============================="
	@echo "Checking C++ code"
	@$(MAKE) install
	@rm -f extended-tests-results/*.log
	@rm -f extended-tests-results/check-results.md
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/check_loop.sh'
	@echo "==============================="

check-single:
	@$(MAKE) clean
	@$(MAKE) install
	@$(MAKE) register
	@Rscript -e 'devtools::install("./extended-tests/cpp4rtest")'
	@Rscript -e 'devtools::check("./extended-tests/cpp4rtest", error_on = "error")'
	
bench:
	@rm -f extended-tests-results/*.rds
	@rm -f extended-tests-results/bench_summary.md
	@$(MAKE) clean
	@$(MAKE) install
	@$(MAKE) register
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/bench_loop.sh'
	@Rscript './scripts/combine-benchmarks.R'

clang_format=`which clang-format-18`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cpp')
	@${clang_format} -i $?
