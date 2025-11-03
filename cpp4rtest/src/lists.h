/* roxygen
@title List functions
@rdname testing-list
@keywords internal
*/
[[cpp4r::register]] list cpp4r_named_list_push_back_() {
  writable::list result;

  double one = 1.0, two = 2.0, three = 3.0;
  // int four = 4;
  // bool five = false;
  // const char* six = "six";
  std::string seven = "seven";
  std::vector<int> eight = {8, 8, 8};
  std::vector<double> nine = {9.0, 9.0, 9.0};
  std::vector<std::string> ten = {"ten", "ten"};

  result.push_back({"one"_nm = one});  // Using literal + assignment
  result.push_back(named_arg("two", two));  // Using named_arg constructor
  result.push_back({"three"_nm = three});  // Using literal + assignment
  result.push_back({"four"_nm = 4});  // Using literal + assignment
  result.push_back({"five"_nm = false});  // Using literal + assignment
  result.push_back({"six"_nm = "six"});  // Using literal + assignment
  result.push_back({"seven"_nm = std::string("seven")});  // Using literal + assignment
  result.push_back({"eight"_nm = std::vector<int>{8, 8, 8}});  // Using literal + assignment + std::vector
  result.push_back({"nine"_nm = std::vector<double>{9.0, 9.0, 9.0}});  // Using literal + assignment + std::vector
  result.push_back({"ten"_nm = std::vector<std::string>{"ten", "ten"}});  // Using literal + assignment + std::vector

  return result;
}

/* roxygen
@title List functions
@rdname testing-list
@keywords internal
*/
[[cpp4r::register]] list cpp4r_named_list_c_style_() {
  writable::list result(10);  // Preallocate list of size 10

  double one = 1.0, two = 2.0, three = 3.0;
  int four = 4;
  bool five = false;
  const char* six = "six";
  std::string seven = "seven";
  std::vector<int> eight = {8, 8, 8};
  std::vector<double> nine = {9.0, 9.0, 9.0};
  std::vector<std::string> ten = {"ten", "ten"};

  result[0] = one;
  result[1] = two;
  result[2] = three;
  result[3] = four;
  result[4] = five;
  result[5] = six;
  result[6] = seven;
  result[7] = eight;
  result[8] = nine;
  result[9] = ten;

  result.names() = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
  
  return result;
}
