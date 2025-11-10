#define CPP4R_USE_FMT
#define FMT_HEADER_ONLY
#include "fmt/core.h"
#include "fmt/format.h"

[[cpp4r::register]] void my_stop(std::string mystring, int myarg) {
  cpp4r::stop(fmt::format(fmt::runtime(mystring), myarg));
}
[[cpp4r::register]] void my_stop_n1(std::string mystring) { cpp4r::stop(mystring); }
[[cpp4r::register]] void my_warning(std::string mystring, std::string myarg) {
  cpp4r::warning(fmt::format(fmt::runtime(mystring), myarg));
}
[[cpp4r::register]] void my_warning_n1(std::string mystring) { cpp4r::warning(mystring); }
[[cpp4r::register]] void my_message(std::string mystring, std::string myarg) {
  cpp4r::message(fmt::format(fmt::runtime(mystring), myarg));
}
[[cpp4r::register]] void my_message_n1(std::string mystring) { cpp4r::message(mystring); }
