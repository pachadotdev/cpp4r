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
	@Rscript -e 'cpp4r::unvendor("./extended-tests/cpp4r4rtest/src/vendor");
		cpp4r::vendor("./extended-tests/cpp4r4rtest/src/vendor")'

docs:
	@Rscript -e 'devtools::document("./"); pkgsite::build_site("./")'

check:
	@echo "==============================="
	@echo "Checking R code"
	@$(MAKE) clean
	@$(MAKE) install
	@$(MAKE) register
	@Rscript -e 'devtools::check("./", error_on = "error")'
	@echo "==============================="
	@echo "Checking C++ code"
	@$(MAKE) install
	@rm -f extended-tests-results/*.log
	@rm -f extended-tests-results/check-results.md
	@Rscript -e	'cpp4r::register("extended-tests/cpp4rtest")'
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/check_loop.sh'
	@echo "==============================="
	
bench:
	@rm -f extended-tests-results/*.rds
	@rm -f extended-tests-results/bench_summary.md
	@$(MAKE) clean
	@$(MAKE) install
	@$(MAKE) register
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/bench_loop.sh'
	@Rscript './scripts/combine-benchmarks.R'

STANDARDS := cxx11 cxx14 cxx17 cxx20 cxx23
COMPILERS := gcc clang

define run-check
check-$(1)-$(2):
	@echo "Checking C++ code with $(1) standard and $(2) compiler"
	@$$(MAKE) install
	@if [ "$(2)" = "clang" ]; then export USE_CLANG=1; else unset USE_CLANG; fi; \
	./scripts/check_prepare.sh "$(1)" "$(2)"; \
	if ! ./scripts/check_run.sh "$(1)" "$(2)"; then \
		echo "Check failed"; \
		./scripts/check_restore.sh "$(1)" "$(2)"; \
		exit 1; \
	fi; \
	./scripts/check_restore.sh "$(1)" "$(2)"
endef

define run-bench
bench-$(1)-$(2):
	@echo "Benchmarking C++ code with $(1) standard and $(2) compiler"
	@$$(MAKE) install
	@if [ "$(2)" = "clang" ]; then export USE_CLANG=1; else unset USE_CLANG; fi; \
	if ! ./scripts/bench_prepare.sh "$(1)" "$(2)"; then \
		echo "Prepare failed"; \
		./scripts/bench_restore.sh "$(1)" "$(2)"; \
		exit 1; \
	fi; \
	if ! ./scripts/bench_install.sh "$(1)"; then \
		echo "Install failed"; \
		./scripts/bench_restore.sh "$(1)" "$(2)"; \
		exit 1; \
	fi; \
	if ! ./scripts/bench_run.sh "$(1)" "$(2)"; then \
		echo "Run failed"; \
		./scripts/bench_restore.sh "$(1)" "$(2)"; \
		exit 1; \
	fi; \
	./scripts/bench_restore.sh "$(1)" "$(2)"
endef

$(foreach std,$(STANDARDS),$(foreach comp,$(COMPILERS),$(eval $(call run-check,$(std),$(comp)))))
$(foreach std,$(STANDARDS),$(foreach comp,$(COMPILERS),$(eval $(call run-bench,$(std),$(comp)))))

# Aliases
$(foreach std,$(STANDARDS),$(eval check-$(std)-glang: check-$(std)-clang))
$(foreach std,$(STANDARDS),$(eval bench-$(std)-glang: bench-$(std)-clang))
$(foreach comp,$(COMPILERS) glang,$(eval check-cxx21-$(comp): check-cxx11-$(comp)))
$(foreach comp,$(COMPILERS) glang,$(eval bench-cxx21-$(comp): bench-cxx11-$(comp)))

clang_format=`which clang-format-18`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cpp')
	@${clang_format} -i $?
