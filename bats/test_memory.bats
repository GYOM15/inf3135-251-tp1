setup() {
  load 'bats-support/load'
  load 'bats-assert/load'
  root_dir="$(cd "$( dirname "$BATS_TEST_FILENAME" )/.." >/dev/null 2>&1 && pwd)"
  PATH="$root_dir/bin:$PATH"
  examples_dir="$root_dir/examples"
  valgrind="valgrind --leak-check=full --error-exitcode=1"
}

@test "kover bounding-box handles memory correctly on a given scene" {
  valgrind kover bounding-box < "$examples_dir"/3b2a.scene
}

@test "kover describe handles memory correctly on a given scene" {
  valgrind kover describe < "$examples_dir"/3b2a.scene
}

@test "kover summarize handles memory correctly on a given scene" {
  valgrind kover summarize < "$examples_dir"/3b2a.scene
}
