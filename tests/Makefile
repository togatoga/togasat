CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I..

.PHONY: all test clean

all: run_tests

run_tests: test_main.cpp test_lit.cpp test_clause.cpp test_solver.cpp doctest.h ../togasat.hpp
	$(CXX) $(CXXFLAGS) test_main.cpp -o run_tests

test: run_tests
	./run_tests

clean:
	rm -f run_tests
