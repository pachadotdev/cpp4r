# cpp4rsimplexphase2

See the [blog post](https://pacha.dev/blog/2023/07/18/cpp4r-simplex/).

The simplex algorithm is well described in [Introduction to Linear Optimization](http://athenasc.com/linoptbook.html) and there is efficient software to solve this. The goal here is to show how to print the intermediate steps of the algorithm, so that it can be used to practice the simplex algorithm for undergraduate students.

Consider a minimization problem of the form:

min c^t x
s.t. Ax >= b, x_i >= 0

where A is a m x n matrix, b is a m x 1 vector, c is a n x 1 vector, and x is a n x 1 vector.

This problem can be organized in a table of the form

```
x_1  ... x_n  |
--------------|----
c_1  ... c_n  | -z
a_11 ... a_1n | b_1
...           | ...
a_m1 ... a_mn | b_m
```

where  c_1,...,c_n are the coefficients of the objective function (i.e., costs), a_11,...,a_mn are the coefficients of the constraints, and b_1,...,b_m are the right-hand side of the constraints.

The simplex algorithm to solve the problem consists in the next steps:

1. If c_j >= 0 for all j, then the current solution is optimal. Basic variables are equal to b_i and non-basic variables are equal to 0.

2. If c_j < 0 for some j, we choose it to enter the base. We chose the variable with the most negative c_j, let's say that it is j = s.

3. If a_is <= 0 for all i, then the problem is unbounded.

4. If a_is > 0 for some i, we choose i = r such that b_r / a_rs = min(b_i / a_is, a_is > 0) and pivot on a_rs, to then go back to step 1.

The coefficients are updated according to:

```
a_ij <- a_ij - a_is * a_rj / a_rs for j != s
a_rj <- a_rj / a_rs
b_i  <- b_i - a_is * b_r / a_rs for i != r
b_r  <- b_r / a_rs
c_j  <- c_j - c_s * a_rj / a_rs
-z   <- -z - c_s * b_r / a_rs
```

This algorithm is equivalent to Gauss method to solve linear systems.

What's wrong with this code?

`devtools::check()` shows this:

```r
❯ checking C++ specification ... NOTE
    Specified C++11: please drop specification unless essential

❯ checking compiled code ... NOTE
  File ‘cpp4rsimplexphase2/libs/cpp4rsimplexphase2.so’:
    Found ‘_ZSt4cout’, possibly from ‘std::cout’ (C++)
      Object: ‘code.o’
  
  Compiled code should not call entry points which might terminate R nor
  write to stdout/stderr instead of to the console, nor use Fortran I/O
  nor system RNGs nor [v]sprintf.
  
  See ‘Writing portable packages’ in the ‘Writing R Extensions’ manual.
```

This also goes against the DRY (don't repeat yourself) logic. I wrote the
first step of the simplex, and then wrote a while loop for steps 2,3,...,N
for clarity. Certainly, this is not an efficient code, it's just a tiny
computational exercise.
