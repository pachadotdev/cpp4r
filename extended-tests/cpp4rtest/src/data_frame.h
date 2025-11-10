/* roxygen
@title Data frame functions
@rdname testing-data-frame
@keywords internal
*/
[[cpp4r::register]] SEXP data_frame_() {
  using namespace cpp4r::literals;
  cpp4r::writable::list out({
      "nums"_nm = {1, 2, 3},
      "letters"_nm = {"x", "y", "z"},
  });

  out.attr("class") = "data.frame";
  out.attr("row.names") = {NA_INTEGER, -3};

  return out;
}
