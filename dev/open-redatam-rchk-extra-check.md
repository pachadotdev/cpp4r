on 2026-05-15 from Kurt Hornik

Specifically, please see the compilation WARNINGs for the
r-devel-linux-x86_64-debian-gcc check flavor.

These are from switching the check to use the GCC 16 compilers from
current Debian testing (which are based on a prerelease from 1 week
before the May 1 release).

Package redatam version 2.2.0
Package built using 89451/R 4.6.0; x86_64-pc-linux-gnu; 2026-02-23 06:21:57 UTC; unix   
Checked with rchk version 35618ebbccf3cd0b45a3530e6303970a22a9056b LLVM version 14.0.6
More information at https://github.com/kalibera/cran-checks/blob/master/rchk/PROTECT.md
For rchk in docker image see https://github.com/kalibera/rchk/blob/master/doc/DOCKER.md

Function cpp4r::writable::r_vector<cpp4r::r_string>::r_vector(long)
  [UP] calling allocating function cpp4r::writable::r_vector<cpp4r::r_string>::r_vector(SEXPREC*, cpp4r::writable::fresh_allocation_tag) with argument allocated using decltype (((std::__1::declval<SEXPREC* (*)(unsigned int, long)>)())((std::declval<unsigned int&&>)(), (std::declval<long const&>)())) cpp4r::protect::function<SEXPREC* (unsigned int, long)>::operator()<unsigned int, long const&>(unsigned int&&, long const&) const cpp4r/include/cpp4r/r_vector_writable_impl.hpp:198
