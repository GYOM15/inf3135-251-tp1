setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  examples_dir="$root_dir/examples"
}

# Normal usage
# ------------

@test "kover describe runs correctly on an empty scene" {
  run kover describe < "$examples_dir"/empty.scene
  assert_success
  assert_output "An empty scene"
}

@test "kover describe runs correctly on a scene with 1 building" {
  run kover describe < "$examples_dir"/1b.scene
  assert_success
  assert_line --index 0 "A scene with 1 building"
  assert_line --index 1 "  building b1 at 0 0 with dimensions 1 1"
}

@test "kover describe runs correctly on a scene with 2 buildings" {
  run kover describe < "$examples_dir"/2b_rev.scene
  assert_success
  assert_line --index 0 "A scene with 2 buildings"
  assert_line --index 1 "  building b1 at 0 0 with dimensions 1 1"
  assert_line --index 2 "  building b2 at 5 8 with dimensions 2 3"
}

@test "kover describe runs correctly on a scene with 3 buildings in reverse order" {
  run kover describe < "$examples_dir"/3b_rev.scene
  assert_success
  assert_line --index 0 "A scene with 3 buildings"
  assert_line --index 1 "  building b1 at 0 0 with dimensions 1 1"
  assert_line --index 2 "  building b2 at 5 8 with dimensions 2 3"
  assert_line --index 3 "  building b3 at -2 -2 with dimensions 1 1"
}

@test "kover describe runs correctly on a scene with 1 antenna" {
  run kover describe < "$examples_dir"/1a.scene
  assert_success
  assert_line --index 0 "A scene with 1 antenna"
  assert_line --index 1 "  antenna a1 at 0 0 with range 1"
}

@test "kover describe runs correctly on a scene with 2 antennas" {
  run kover describe < "$examples_dir"/2a.scene
  assert_success
  assert_line --index 0 "A scene with 2 antennas"
  assert_line --index 1 "  antenna a1 at 0 0 with range 1"
  assert_line --index 2 "  antenna a2 at 2 3 with range 5"
}

@test "kover describe runs correctly on a scene with 2 antennas in reverse order" {
  run kover describe < "$examples_dir"/2a_rev.scene
  assert_success
  assert_line --index 0 "A scene with 2 antennas"
  assert_line --index 1 "  antenna a1 at 0 0 with range 1"
  assert_line --index 2 "  antenna a2 at 2 3 with range 5"
}

@test "kover describe runs correctly on a scene with 1 building and 1 antenna" {
  run kover describe < "$examples_dir"/1b1a.scene
  assert_success
  assert_line --index 0 "A scene with 1 building and 1 antenna"
  assert_line --index 1 "  building b1 at 0 0 with dimensions 1 1"
  assert_line --index 2 "  antenna a1 at 2 3 with range 5"
}

# Wrong lines
# -----------

@test "kover describe reports an error when first line is invalid" {
  run kover describe < "$examples_dir"/first_line.invalid
  [ "$status" -eq 1 ]
  assert_output "error: first line must be exactly 'begin scene'"
}

@test "kover describe reports an error when a line is unrecognized" {
  run kover describe < "$examples_dir"/unrecognized_line.invalid
  [ "$status" -eq 1 ]
  assert_output "error: unrecognized line (line #2)"
}

@test "kover describe reports an error when last line is not 'end scene'" {
  run kover describe < "$examples_dir"/no_end_scene.invalid
  [ "$status" -eq 1 ]
  assert_output "error: last line must be exactly 'end scene'"
}

# Wrong buildings
# ---------------

@test "kover describe reports an error when two buildings have same id" {
  run kover describe < "$examples_dir"/2b_non_unique_id.invalid
  [ "$status" -eq 1 ]
  assert_output "error: building identifier b1 is non unique"
}

@test "kover describe reports an error when two buildings are overlapping" {
  run kover describe < "$examples_dir"/2b_overlapping.invalid
  [ "$status" -eq 1 ]
  assert_output "error: buildings b1 and b2 are overlapping"
}

@test "kover describe reports an error when a building line has a wrong number of arguments" {
  run kover describe < "$examples_dir"/1b_wrong_number_of_arguments.invalid
  [ "$status" -eq 1 ]
  assert_output "error: building line has wrong number of arguments (line #2)"
}

@test "kover describe reports an error when a building line has an invalid identifier" {
  run kover describe < "$examples_dir"/1b_wrong_id.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid identifier "b^" (line #2)'
}

@test "kover describe reports an error when a building line has an invalid x" {
  run kover describe < "$examples_dir"/1b_wrong_x.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "a" (line #2)'
}

@test "kover describe reports an error when a building line has an invalid y" {
  run kover describe < "$examples_dir"/1b_wrong_y.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "02" (line #2)'
}

@test "kover describe reports an error when a building line has an invalid w" {
  run kover describe < "$examples_dir"/1b_wrong_w.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid positive integer "c" (line #2)'
}

@test "kover describe reports an error when a building line has an invalid h" {
  run kover describe < "$examples_dir"/1b_wrong_h.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid positive integer "-2" (line #2)'
}

# Wrong antennas
# --------------

@test "kover describe reports an error when two antennas have the same position" {
  run kover describe < "$examples_dir"/2a_same_position.invalid
  [ "$status" -eq 1 ]
  assert_output "error: antennas a1 and a2 have the same position"
}

@test "kover describe reports an error when an antenna line has the wrong number of arguments" {
  run kover describe < "$examples_dir"/1a_wrong_number_of_arguments.invalid
  [ "$status" -eq 1 ]
  assert_output "error: antenna line has wrong number of arguments (line #2)"
}

@test "kover describe reports an error when an antenna line has an invalid identifier" {
  run kover describe < "$examples_dir"/1a_wrong_id.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid identifier "a^" (line #2)'
}

@test "kover describe reports an error when a antenna line has an invalid x" {
  run kover describe < "$examples_dir"/1a_wrong_x.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "00" (line #2)'
}

@test "kover describe reports an error when a antenna line has an invalid y" {
  run kover describe < "$examples_dir"/1a_wrong_y.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "2t" (line #2)'
}

@test "kover describe reports an error when a antenna line has an invalid r" {
  run kover describe < "$examples_dir"/1a_wrong_r.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid positive integer "-1" (line #2)'
}
