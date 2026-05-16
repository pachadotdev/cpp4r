[[cpp4r::register]] cpp4r::writable::doubles grow_(R_xlen_t n) {
  cpp4r::writable::doubles x;
  R_xlen_t i = 0;
  while (i < n) {
    x.push_back(i++);
  }

  return x;
}

[[cpp4r::register]] cpp4r::writable::complexes grow_cplx_(R_xlen_t n) {
  cpp4r::writable::complexes x;
  R_xlen_t i = 0;
  while (i < n) {
    x.push_back(std::complex<double>(i, i));
    i++;
  }

  return x;
}
