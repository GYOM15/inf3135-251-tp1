setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  examples_dir="$root_dir/examples"
}

# Normal usage
# ------------

@test "kover help prints the usage on stdout" {
  run kover help
  assert_success
  assert_line --index 0 "Usage: kover SUBCOMMAND"
  assert_line --index 1 \
    "Handles positioning of communication antennas by reading a scene on stdin."
}
