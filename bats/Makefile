.PHONY: count test

test:
	bats-core/bin/bats test_kover.bats
	bats-core/bin/bats test_bounding_box.bats
	bats-core/bin/bats test_describe.bats
	bats-core/bin/bats test_help.bats
	bats-core/bin/bats test_summarize.bats

count:
	bats-core/bin/bats -c test_kover.bats
	bats-core/bin/bats -c test_bounding_box.bats
	bats-core/bin/bats -c test_describe.bats
	bats-core/bin/bats -c test_help.bats
	bats-core/bin/bats -c test_memory.bats
	bats-core/bin/bats -c test_summarize.bats
