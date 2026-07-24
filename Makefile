clean:
	@Rscript -e 'tinydev::pkg_clean(".");'

install:
	@Rscript -e 'tinydev::pkg_install(".");'

check:
	@Rscript -e 'tinydev::pkg_check(".");'
	@Rscript -e 'tinydev::pkg_install(".");'
	@Rscript -e 'tinydev::pkg_check("./cpp4rtest");'

site:
	@Rscript -e 'tinydev::pkg_document(".");'
	@Rscript -e 'pkgsite::build_site(".");'
	python -m http.server --directory docs

# CXX_STDS := cxx11 cxx14 cxx17 cxx20 cxx23
CXX_STDS := cxx17 cxx20 cxx23
CXX_COMPILERS := gcc clang

# Loop the single-standard check below over every supported C++ standard and
# compiler.
check-cxx:
	@chmod +x ./scripts/check.sh
	@for std in $(CXX_STDS); do \
		for cc in $(CXX_COMPILERS); do \
			./scripts/check.sh ubuntu-release $$std $$cc || exit 1; \
		done; \
	done

# make check-c11-gcc, check-c14-gcc, check-c17-gcc, check-c20-gcc, check-c23-gcc:
# quick Docker check pinning GCC to a single C++ standard.
check-cxx%-gcc:
	@chmod +x ./scripts/check.sh
	@./scripts/check.sh ubuntu-release cxx$* gcc

# make check-c11-clang, check-c14-clang, check-c17-clang, check-c20-clang, check-c23-clang:
# quick Docker check pinning clang to a single C++ standard.
check-cxx%-clang:
	@chmod +x ./scripts/check.sh
	@./scripts/check.sh ubuntu-release cxx$* clang

# make check-cran-<image>, e.g. check-cran-gcc16, check-cran-rocky8:
# full CRAN-style check via Docker using <image>'s default toolchain.
check-cran-%:
	@chmod +x ./scripts/check.sh
	@./scripts/check.sh $*

# CRAN-like containers (pair: CRAN name : r-hub image)
CRAN_EXTRA_PAIRS := \
	r-devel-linux-x86_64-debian-clang:ubuntu-clang \
	r-devel-linux-x86_64-debian-gcc:ubuntu-gcc15 \
	r-patched-linux-x86_64:ubuntu-next \
	r-release-linux-x86_64:ubuntu-release

# Extra CRAN check images
CRAN_EXTRA := atlas clang-asan clang-ubsan clang21 clang22 donttest \
	gcc16 gcc-asan lto mkl nold nosuggests rchk valgrind

# Loop the single-image CRAN check above over every image in CRAN_EXTRA_PAIRS
# (the r-hub image is the part of each pair after the colon).
check-cran:
	@chmod +x ./scripts/check.sh
	@for pair in $(CRAN_EXTRA_PAIRS); do \
		image=$${pair#*:}; \
		./scripts/check.sh $$image || exit 1; \
	done

# Loop the single-image CRAN check above over every image in CRAN_EXTRA.
check-cran-extra:
	@chmod +x ./scripts/check.sh
	@for image in $(CRAN_EXTRA); do \
		./scripts/check.sh $$image || exit 1; \
	done

clang_format=`which clang-format-21`

format: $(shell find . -name '*.h') $(shell find . -name '*.hpp') $(shell find . -name '*.cxx')
	@${clang_format} -i $?

build-r-devel:
	@echo "Building R-devel from source"
	./scripts/build_r_devel.sh

check-devel:
	@echo "Checking with R-devel (CXX23, gcc)"
	./scripts/check_r_devel.sh cxx23 gcc

$(foreach std,$(STANDARDS),$(foreach comp,$(COMPILERS),$(eval $(call run-check,$(std),$(comp)))))
$(foreach std,$(STANDARDS),$(eval check-local-$(std)-glang: check-local-$(std)-clang))
