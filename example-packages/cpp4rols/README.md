
<!-- README.md is generated from README.Rmd. Please edit that file -->

# cpp4rols

<!-- badges: start -->

<!-- badges: end -->

The goal of cpp4rdummyols is to show how to use cpp4r and solve a
specific Linear Algebra problem, which is to obtain the OLS estimator,
in the most naive way Use this only for elemental examples, not for
actual estimation. There is a good explanation about why R's lm()
function uses a QR decomposition instead of a naive computation of the
form (X’X)-1(X’Y) as I did here. See
<http://madrury.github.io/jekyll/update/statistics/2016/07/20/lm-in-R.html>.

## Installation

You can install the development version of cpp4rdummyols like so:

``` r
remotes::install_github("pachadotdev/cpp4rdummyols")
```

## Example

This is a basic example which shows you how to solve a common problem:

``` r
library(cpp4rols)

# estimate the model mpg = intercept + slope * wt + error

X <- matrix(1, nrow = nrow(mtcars), ncol = 2)
X[, 2] <- mtcars$wt

Y <- matrix(mtcars$mpg, nrow = nrow(mtcars), ncol = 1)

naive_ols(X, Y)
#>           [,1]
#> [1,] 37.285126
#> [2,] -5.344472
```

Now let's compare with the `lm()` function from R:

``` r
coef(lm(mpg ~ wt, data = mtcars))
#> (Intercept)          wt 
#>   37.285126   -5.344472
```
