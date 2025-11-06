# R <-> C++ Benchmark 2.6 (11/2025) [Mauricio Vargas Sepulveda]
# Based on R Benchmark 2.5 by Simon Urbanek

# Laptop specifications for the benchmarking
# Operating System: Manjaro Linux
# KDE Plasma Version: 6.3.6
# KDE Frameworks Version: 6.18.0
# Qt Version: 6.9.2
# Kernel Version: 6.12.48-1-MANJARO (64-bit)
# Graphics Platform: X11
# Processors: 8 × 11th Gen Intel® Core™ i7-1185G7 @ 3.00GHz
# Memory: 15.3 GiB of RAM
# Graphics Processor: Intel® Iris® Xe Graphics
# Manufacturer: LENOVO
# Product Name: 20XW003KUS
# System Version: ThinkPad X1 Carbon Gen 9

set.seed(42)

make_bench_data <- function() {
  # scaling factor
  k <- 2L

  # I. Matrix Calculation
  # Test 1: Creation, transpose, deformation of a 2500k x 2500k matrix
  mm_n <- 2500 * k
  mm_div <- 10 * k
  
  # Test 2: 2400k x 2400k normal distributed random matrix ^1000
  mp_n <- 2400 * k
  mp_div <- 2 * k
  mp_exp <- 1000 * k
  
  # Test 3: Sorting of 7,000,000k random values
  sort_n <- 7000000 * k
  
  # Test 4: 2800k x 2800k cross-product matrix (b = a' * a)
  cp_n <- 2800 * k
  
  # Test 5: Linear regression over a 2000k x 2000k matrix
  lr_nrow <- 2000 * k
  lr_ncol <- 2000 * k
  
  # II. Matrix Functions
  # Test 1: FFT over 2,400,00k random values
  fft_n <- 2400000 * k
  
  # Test 2: Eigenvalues of a 640k x 640k random matrix
  eigen_n <- 640 * k
  
  # Test 3: Determinant of a 2500k x 2500k random matrix
  det_n <- 2500 * k
  
  # Test 4: Cholesky decomposition of a 3000k x 3000k matrix
  chol_n <- 3000 * k
  
  # Test 5: Inverse of a 1600k x 1600k random matrix
  inv_n <- 1600 * k
  
  # III. Programming
  # Test 1: 3,500,000k Fibonacci numbers calculation (vector calc)
  fib_n <- 3500000 * k
  fib_scale <- 1000 * k
  
  # Test 2: Creation of a 3000k x 3000k Hilbert matrix (matrix calc)
  hilbert_n <- 3000 * k
  
  # Test 3: Grand common divisors of 400,000k pairs (recursion)
  gcd_n <- 400000 * k
  gcd_scale <- 1000 * k
  
  # Test 4: Creation of a 500k x 500k Toeplitz matrix (loops)
  toeplitz_n <- 500 * k
  
  # Test 5: Escoufier's method on a 45k x 45k matrix (mixed)
  esc_n <- 45 * k

  res <- list(
    # I. Matrix Calculation
    matrix_manip_vec = rnorm(mm_n * mm_n) / mm_div,
    matrix_manip_nrow = mm_n,
    matrix_manip_ncol = mm_n,

    matrix_power = abs(matrix(rnorm(mp_n * mp_n) / mp_div, ncol = mp_n, nrow = mp_n)),
    power_exponent = mp_exp,

    sort_vec = rnorm(sort_n),

    crossprod_mat = matrix(rnorm(cp_n * cp_n), ncol = cp_n, nrow = cp_n),

    linreg_X = matrix(rnorm(lr_nrow * lr_ncol), ncol = lr_ncol, nrow = lr_nrow),
    linreg_y = rnorm(lr_nrow),

    # II. Matrix Functions
    fft_vec = rnorm(fft_n),
    
    eigen_mat = matrix(rnorm(eigen_n * eigen_n), ncol = eigen_n, nrow = eigen_n),
    
    det_mat = matrix(rnorm(det_n * det_n), ncol = det_n, nrow = det_n),
    
    chol_mat = crossprod(matrix(rnorm(chol_n * chol_n), ncol = chol_n, nrow = chol_n)),
    
    inv_mat = matrix(rnorm(inv_n * inv_n), ncol = inv_n, nrow = inv_n),

    # III. Programming
    fib_indices = floor(runif(fib_n) * fib_scale),
    
    hilbert_n = hilbert_n,
    
    gcd_x = as.integer(ceiling(runif(gcd_n) * gcd_scale)),
    gcd_y = as.integer(ceiling(runif(gcd_n) * gcd_scale)),
    
    toeplitz_n = toeplitz_n,
    
    escoufier_mat = abs(matrix(rnorm(esc_n * esc_n), ncol = esc_n, nrow = esc_n))
  )

  saveRDS(res, file = "./bench_data.rds", compress = "xz")
}

make_bench_data()
