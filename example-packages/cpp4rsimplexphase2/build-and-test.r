# build

devtools::clean_dll()
cpp4r::cpp_register()
devtools::document()
devtools::load_all()

# test

c <- c(-1, -3)
b <- c(3, 2)

A <- matrix(
    c(1, -3, 1, 1),
    nrow = 2,
    ncol = 2,
    byrow = FALSE
)

cpp4r_simplex_phase2(c, b, A)
