/* roxygen
@title Errors
@description Test suite
@param mystring string to include in the error/warning/message
@export
*/
[[cpp4r::register]] void my_stop_n1_(std::string mystring) { cpp4r::stop(mystring); }

/* roxygen
@title Errors with Two Arguments
@description Test suite
@param mystring string to include in the error
@param myarg additional string to include in the error
@export
*/
[[cpp4r::register]] void my_stop_n2_(std::string mystring, std::string myarg) {
  cpp4r::stop(mystring, myarg.c_str());
}

/* roxygen
@title Warnings with One Argument
@description Test suite
@param mystring string to include in the warning
@export
*/
[[cpp4r::register]] void my_warning_n1_(std::string mystring) {
  cpp4r::warning(mystring);
}

/* roxygen
@title Warnings with Two Arguments
@description Test suite
@param mystring string to include in the warning
@param myarg additional string to include in the warning
@export
*/
[[cpp4r::register]] void my_warning_n2_(std::string mystring, std::string myarg) {
  cpp4r::warning(mystring, myarg.c_str());
}

/* roxygen
@title Messages with One Argument
@description Test suite
@param mystring string to include in the message
@export
*/
[[cpp4r::register]] void my_message_n1_(std::string mystring) {
  cpp4r::message(mystring);
}

/* roxygen
@title Messages with Two Arguments
@description Test suite
@param mystring string to include in the message
@param myarg additional string to include in the message
@export
*/
[[cpp4r::register]] void my_message_n2_(std::string mystring, std::string myarg) {
  cpp4r::message(mystring, myarg.c_str());
}
