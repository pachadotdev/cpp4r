#' @title Title: Header-Only 'C++' and 'R' Interface
#' @description Provides a header only, 'C++' interface to 'R' with enhancements
#'  over 'cpp11'. Enforces copy-on-write semantics consistent with 'R'
#'  behavior. Offers native support for ALTREP objects, 'UTF-8' string
#'  handling, modern 'C++' features and idioms, and reduced memory
#'  requirements. Allows for vendoring, making it useful for restricted
#'  environments. Compared to 'cpp11', it adds support for converting 'C++'
#'  maps to 'R' lists, 'Roxygen' documentation directly in 'C++' code, proper
#'  handling of matrix attributes, support for nullable external pointers,
#'  bidirectional copy of complex number types, flexibility in type
#'  conversions, use of nullable pointers, and various performance
#'  optimizations.
"_PACKAGE"
