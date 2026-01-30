.PHONY: all test clean format

all: test

test:
	$(MAKE) -C tests test

clean:
	$(MAKE) -C tests clean

format:
	clang-format -i togasat.hpp
	clang-format -i example/sudoku/main.cpp
	clang-format -i tests/test_*.cpp
