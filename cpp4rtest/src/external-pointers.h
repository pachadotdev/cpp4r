// Pacha: Test nullable external_pointer (#312)

/* roxygen
@title Test Nullable External Pointer on 'C++' Side (nullptr)
@description Test suite
@export
*/
[[cpp4r::register]] cpp4r::external_pointer<int> nullable_extptr_1() {
  return cpp4r::external_pointer<int>(nullptr);
}

/* roxygen
@title Test Nullable External Pointer on 'C++' Side (R_NilValue)
@description Test suite
@export
*/
[[cpp4r::register]] cpp4r::external_pointer<int> nullable_extptr_2() {
  return cpp4r::external_pointer<int>(R_NilValue);
}
