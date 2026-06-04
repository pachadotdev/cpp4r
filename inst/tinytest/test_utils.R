# Tests for collapse_data() and stop_unless_installed()

# --- collapse_data ---

expect_equal(cpp4r:::collapse_data(mtcars, ""), "")
expect_equal(cpp4r:::collapse_data(mtcars[FALSE, ], "{hp}"), "")
expect_equal(cpp4r:::collapse_data(mtcars[1, ], "{hp}"), "110")
expect_equal(cpp4r:::collapse_data(mtcars[1:2, ], "{hp}"), "110, 110")
