exec = bin/kover
main = src/kover.c

.PHONY: bindir build clean test

$(exec): bindir $(main)
	gcc $(main) -o $(exec)

build: $(exec)

bindir:
	mkdir -p bin

clean:
	rm -rf bin

test: $(exec)
	$(MAKE) -C bats
