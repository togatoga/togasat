all:
	g++ -std=c++11 -o Solver -O3 Solver.cpp
clean:
	rm Solver
