clean:
	@Rscript -e 'devtools::clean_dll("cpp4rtest");'

install:
	@Rscript -e 'devtools::install(".", upgrade = FALSE)'

docs:
	@Rscript -e 'devtools::document("./"); pkgsite::build_site("./")'
	
bench:
	@rm -f extended-tests-results/*.rds
	@rm -f extended-tests-results/bench_summary.md
	@$(MAKE) clean
	@$(MAKE) install
	@export -p USE_CLANG; /bin/bash -euo pipefail -c './scripts/bench_loop.sh'
	@Rscript './scripts/combine-benchmarks.R'

STANDARDS := cxx17 cxx20 cxx23
COMPILERS := gcc clang

ALL_CHECKS := $(foreach std,$(STANDARDS),$(foreach comp,$(COMPILERS),check-$(std)-$(comp)))

check: $(ALL_CHECKS)

define run-check
check-$(1)-$(2):
	@echo "Checking C++ code with $(1) standard and $(2) compiler"
	# @$$(MAKE) install
	./scripts/check_prepare.sh "$(1)" "$(2)"; \
	if ! ./scripts/check_run.sh "$(1)" "$(2)"; then \
		echo "Check failed"; \
		./scripts/check_restore.sh "$(1)" "$(2)"; \
		exit 1; \
	fi; \
	./scripts/check_restore.sh "$(1)" "$(2)"
endef

clang_format=`which clang-format-21`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cpp')
	@${clang_format} -i $?

build-r-devel:
	@echo "Building R-devel from source"
	./scripts/build_r_devel.sh

check-devel:
	@echo "Checking with R-devel (CXX23, gcc)"
	./scripts/check_r_devel.sh cxx23 gcc

$(foreach std,$(STANDARDS),$(foreach comp,$(COMPILERS),$(eval $(call run-check,$(std),$(comp)))))
$(foreach std,$(STANDARDS),$(foreach comp,$(COMPILERS),$(eval $(call run-bench,$(std),$(comp)))))
$(foreach std,$(STANDARDS),$(eval check-$(std)-glang: check-$(std)-clang))
$(foreach std,$(STANDARDS),$(eval bench-$(std)-glang: bench-$(std)-clang))
