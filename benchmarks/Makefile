clean:
	@Rscript -e 'devtools::clean_dll("cpp11benchmark")'
	@Rscript -e 'devtools::clean_dll("cpp4rbenchmark")'
	@Rscript -e 'devtools::clean_dll("Rcppbenchmark")'

bench:
	@clear
	@/bin/bash -euo pipefail -c './bench_loop.sh'
