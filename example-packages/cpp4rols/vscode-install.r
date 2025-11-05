# cpp4r::cpp_vendor() # run only when updating C++ headers
devtools::clean_dll()
cpp4r::cpp_register()
devtools::document()
# devtools::install()
devtools::load_all()
