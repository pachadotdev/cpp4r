
.PHONY: check-cran check-cran-extra check-% clean install docs

LOGDIR := extended-tests-results/logs

# CRAN-like containers (pair: CRAN name : r-hub image)
CRAN_PAIRS := \
	r-devel-linux-x86_64-debian-clang:ubuntu-clang \
 	r-devel-linux-x86_64-debian-gcc:ubuntu-gcc15 \
 	r-patched-linux-x86_64:ubuntu-next \
 	r-release-linux-x86_64:ubuntu-release

# Extra CRAN check images
CRAN_EXTRA := atlas clang-asan clang-ubsan clang21 clang22 donttest \
	gcc16 gcc-asan lto mkl nold nosuggests rchk valgrind

check-cran:
	@mkdir -p $(LOGDIR)
	@chmod +x ./scripts/check.sh
	@for pair in $(CRAN_PAIRS); do \
		cran=$${pair%%:*}; rhub=$${pair##*:}; \
		echo "=== checking $$cran (r-hub: $$rhub) ===" | tee -a $(LOGDIR)/check-$${rhub}.log; \
		./scripts/check.sh $$rhub 2>&1 | sed -u 's/^/  /' | tee -a $(LOGDIR)/check-$${rhub}.log; \
	done

check-cran-extra:
	@mkdir -p $(LOGDIR)
	@chmod +x ./scripts/check.sh
	@for rhub in $(CRAN_EXTRA); do \
		echo "=== checking $$rhub ===" | tee -a $(LOGDIR)/check-$${rhub}.log; \
		./scripts/check.sh $$rhub 2>&1 | sed -u 's/^/  /' | tee -a $(LOGDIR)/check-$${rhub}.log; \
	done

# Individual check target, e.g. `make check-clang22`
check-%:
	@mkdir -p $(LOGDIR)
	@chmod +x ./scripts/check.sh
	@rhub=$*; echo "=== checking $$rhub ===" | tee -a $(LOGDIR)/check-$$rhub.log; \
	./scripts/check.sh $$rhub 2>&1 | sed -u 's/^/  /' | tee -a $(LOGDIR)/check-$$rhub.log

clean:
	@Rscript -e 'devtools::clean_dll("cpp4rtest");'

install:
	@Rscript -e 'devtools::install(".", upgrade = FALSE)'

docs:
	@Rscript -e 'devtools::document("./"); pkgsite::build_site("./")'

clang_format=`which clang-format-21`

format: $(shell find . -not -path './check-docker/*' -name '*.h') $(shell find . -not -path './check-docker/*' -name '*.hpp') $(shell find . -not -path './check-docker/*' -name '*.cpp')
	@${clang_format} -i $?
