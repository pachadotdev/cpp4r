Dear CRAN Team:

I had to patch v0.5.0 because GitHub actions logs were hiding a non-API method
that went unnoticed.

This time I edited my local Makefile to:

1. Build R-devel to /opt
2. Test locally and export the full logs to a txt file

This patch is fully compliant with R-devel 4.7.0 downloaded from
https://cran.r-project.org/src/base-prerelease/R-devel.tar.gz.

Best,
MVS
