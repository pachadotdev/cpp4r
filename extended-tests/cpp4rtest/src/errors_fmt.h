[[cpp4r::register]] void my_stop(std::string mystring, int myarg) {
  cpp4r::stop(mystring);
}
[[cpp4r::register]] void my_stop_n1(std::string mystring) { cpp4r::stop(mystring); }
[[cpp4r::register]] void my_warning(std::string mystring, std::string myarg) {
  cpp4r::warning(mystring);
}
[[cpp4r::register]] void my_warning_n1(std::string mystring) { cpp4r::warning(mystring); }
[[cpp4r::register]] void my_message(std::string mystring, std::string myarg) {
  cpp4r::message(mystring);
}
[[cpp4r::register]] void my_message_n1(std::string mystring) { cpp4r::message(mystring); }
