setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  examples_dir="$root_dir/examples"
}

# Normal usage
# ------------

@test "kover bounding-box runs correctly on an empty scene" {
  run kover bounding-box < "$examples_dir"/empty.scene
  assert_success
  assert_output "undefined (empty scene)"
}

@test "kover bounding-box runs correctly on a scene with 1 building" {
  run kover bounding-box < "$examples_dir"/1b.scene
  assert_success
  assert_output "bounding box [-1, 1] x [-1, 1]"
}

@test "kover bounding-box runs correctly on a scene with 2 buildings" {
  run kover bounding-box < "$examples_dir"/2b.scene
  assert_success
  assert_output "bounding box [-1, 7] x [-1, 11]"
}

@test "kover bounding-box runs correctly on a scene with 1 antenna" {
  run kover bounding-box < "$examples_dir"/1a.scene
  assert_success
  assert_output "bounding box [-1, 1] x [-1, 1]"
}

@test "kover bounding-box runs correctly on a scene with 2 antennas" {
  run kover bounding-box < "$examples_dir"/2a.scene
  assert_success
  assert_output "bounding box [-3, 7] x [-2, 8]"
}

@test "kover bounding-box runs correctly on a scene with 1 building and 1 antenna" {
  run kover bounding-box < "$examples_dir"/1b1a.scene
  assert_success
  assert_output "bounding box [-3, 7] x [-2, 8]"
}

# Wrong lines
# -----------

@test "kover bounding-box reports an error when first line is invalid" {
  run kover bounding-box < "$examples_dir"/first_line.invalid
  [ "$status" -eq 1 ]
  assert_output "error: first line must be exactly 'begin scene'"
}

@test "kover bounding-box reports an error when a line is unrecognized" {  
  run kover bounding-box < "$examples_dir"/unrecognized_line.invalid
  [ "$status" -eq 1 ]
  assert_output "error: unrecognized line (line #2)"
}

@test "kover bounding-box reports an error when last line is not 'end scene'" {
  run kover bounding-box < "$examples_dir"/no_end_scene.invalid
  [ "$status" -eq 1 ]
  assert_output "error: last line must be exactly 'end scene'"
}

# Wrong buildings
# ---------------

@test "kover bounding-box reports an error when two buildings have same id" {
  run kover bounding-box < "$examples_dir"/2b_non_unique_id.invalid
  [ "$status" -eq 1 ]
  assert_output "error: building identifier b1 is non unique"
}

@test "kover bounding-box reports an error when two buildings are overlapping" {
  run kover bounding-box < "$examples_dir"/2b_overlapping.invalid
  [ "$status" -eq 1 ]
  assert_output "error: buildings b1 and b2 are overlapping"
}

@test "kover bounding-box reports an error when a building line has a wrong number of arguments" {
  run kover bounding-box < "$examples_dir"/1b_wrong_number_of_arguments.invalid
  [ "$status" -eq 1 ]
  assert_output "error: building line has wrong number of arguments (line #2)"
}

@test "kover bounding-box reports an error when a building line has an invalid identifier" {
  run kover bounding-box < "$examples_dir"/1b_wrong_id.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid identifier "b^" (line #2)'
}

@test "kover bounding-box reports an error when a building line has an invalid x" {
  run kover bounding-box < "$examples_dir"/1b_wrong_x.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "a" (line #2)'
}

@test "kover bounding-box reports an error when a building line has an invalid y" {
  run kover bounding-box < "$examples_dir"/1b_wrong_y.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "02" (line #2)'
}

@test "kover bounding-box reports an error when a building line has an invalid w" {
  run kover bounding-box < "$examples_dir"/1b_wrong_w.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid positive integer "c" (line #2)'
}

@test "kover bounding-box reports an error when a building line has an invalid h" {
  run kover bounding-box < "$examples_dir"/1b_wrong_h.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid positive integer "-2" (line #2)'
}

# Wrong antennas
# --------------

@test "kover bounding-box reports an error when two antennas have the same position" {
  run kover bounding-box < "$examples_dir"/2a_same_position.invalid
  [ "$status" -eq 1 ]
  assert_output "error: antennas a1 and a2 have the same position"
}

@test "kover bounding-box reports an error when an antenna line has the wrong number of arguments" {
  run kover bounding-box < "$examples_dir"/1a_wrong_number_of_arguments.invalid
  [ "$status" -eq 1 ]
  assert_output "error: antenna line has wrong number of arguments (line #2)"
}

@test "kover bounding-box reports an error when an antenna line has an invalid identifier" {
  run kover bounding-box < "$examples_dir"/1a_wrong_id.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid identifier "a^" (line #2)'
}

@test "kover bounding-box reports an error when a antenna line has an invalid x" {
  run kover bounding-box < "$examples_dir"/1a_wrong_x.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "00" (line #2)'
}

@test "kover bounding-box reports an error when a antenna line has an invalid y" {
  run kover bounding-box < "$examples_dir"/1a_wrong_y.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid integer "2t" (line #2)'
}

@test "kover bounding-box reports an error when a antenna line has an invalid r" {
  run kover bounding-box < "$examples_dir"/1a_wrong_r.invalid
  [ "$status" -eq 1 ]
  assert_output 'error: invalid positive integer "-1" (line #2)'
}
