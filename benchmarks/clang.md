Manjaro uses `gcc` as the default compiler.


```bash
sudo pacman -S clang libc++ libc++abi
```

To be sure that `devtools::install()` uses the correct version of `clang++`, edit `~/.R/Makevars` file:

```bash
CXXFLAGS = -O3
CXX11FLAGS = -O3
CXX14FLAGS = -O3
CXX17FLAGS = -O3
CXX20FLAGS = -O3
CXX23FLAGS = -O3

# Clang flags
CLANGLIB=-stdlib=libc++
CC=$(CCACHE) clang$(CLANGVER)
CXX=$(CCACHE) clang++ $(CLANGLIB)
CXX11=$(CCACHE) clang++$(CLANGLIB)
CXX14=$(CCACHE) clang++$(CLANGLIB)
CXX17$(CCACHE) clang++$(CLANGLIB)
CXX20=$(CCACHE) clang++$(CLANGLIB)
CXX23=$(CCACHE) clang++$(CLANGLIB)
SHLIB_CXXLD=clang++$(CLANGLIB)
```

Comment the "Clang flags" section to compile with `g++` instead of `clang++` again.

Check by closing R and opening it again to check the compiler used by `R CMD SHLIB`:

```
pkgbuild::check_build_tools(debug = TRUE)
```
