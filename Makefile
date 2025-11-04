clean:
	@Rscript -e 'devtools::clean_dll("cpp4rtest"); cpp4r::register("cpp4rtest")'

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
	@/bin/bash -euo pipefail -c './scripts/test_loop.sh'

check:
	@clear
	@echo "==============================="
	@echo "Checking R code"
	@Rscript -e 'devtools::install(); devtools::check(error_on = "error")'
	@clear
	@echo "==============================="
	@echo "Checking C++ code"
	@/bin/bash -euo pipefail -c './scripts/check_loop.sh'
	@echo "==============================="

clang_format=`which clang-format-18`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cpp')
	@${clang_format} -i $?
