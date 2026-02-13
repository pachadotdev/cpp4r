[[cpp4r::register]] void my_stop_n1fmt(std::string mystring) { cpp4r::stop(mystring); }
[[cpp4r::register]] void my_stop_n2fmt(std::string mystring, std::string myarg) {
  cpp4r::stop(mystring, myarg.c_str());
}
[[cpp4r::register]] void my_warning_n1fmt(std::string mystring) {
  cpp4r::warning(mystring);
}
[[cpp4r::register]] void my_warning_n2fmt(std::string mystring, std::string myarg) {
  cpp4r::warning(mystring, myarg.c_str());
}
[[cpp4r::register]] void my_message_n1fmt(std::string mystring) {
  cpp4r::message(mystring);
}
[[cpp4r::register]] void my_message_n2fmt(std::string mystring, std::string myarg) {
  cpp4r::message(mystring, myarg.c_str());
}
