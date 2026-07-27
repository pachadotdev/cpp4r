# Contributing to cpp4r

This outlines how to propose a change to cpp4r.

## Fixing typos

You can fix typos, spelling mistakes, or grammatical errors in the documentation directly using the GitHub web interface, as long
as the changes are made in the _source_ file. This generally means you'll need to edit Roxygen comments in an `.R`, not a `.Rd`
file. You can find the `.R` file that generates the `.Rd` by reading the comment in the first line.

## Proposing (breaking) changes

It is a good idea to open an issue and explain the need for such change. I can work on that change, we can divide some tasks, or
you can send me a pull request after we agree on the need, timeline, etc.

## Fixing bugs

If you have found a bug, please file an issue that illustrates the bug with a minimal example. I will ask for help and ask for clarification as needed.

## Pull request process

* Fork the package and clone onto your computer.
* Make sure the package passes `R CMD check .` from the console or equivalently with `tinydev::pkg_check()` from R.
* Create a Git branch for your pull request (e.g., `git checkout -b fix-grouped-means` from the console).
* Push the changes from the console and create a pull request from GitHub' website.
* The title of your PR should briefly describe the change.
* The body of your PR should contain `Fixes a,b,c,...`.

## Code of Conduct

Please note that the cpp4r project is released with a [Contributor Code of Conduct](CODE_OF_CONDUCT.md). By contributing to this
project you agree to abide by its terms.
