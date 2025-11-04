clean:
	@Rscript -e 'devtools::clean_dll("cpp11benchmark")'
	@Rscript -e 'devtools::clean_dll("cpp4rbenchmark")'
	@Rscript -e 'devtools::clean_dll("Rcppbenchmark")'

bench:
	@clear
	@/bin/bash -euo pipefail -c './bench_loop.sh'

# Run benchmarks for a specific compiler and standard
bench-specific:
	@clear
	@/bin/bash -euo pipefail -c './bench_prepare.sh $(STD) $(COMPILER) && ./bench_install.sh $(STD) && ./bench_run.sh $(STD) $(COMPILER) && ./bench_restore.sh $(STD) $(COMPILER)'
