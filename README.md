# Togasat
CDCL SAT Solver for programming contest.  
Togasat is CDCL SAT Solver by modifying minisat.

##Install
`g++ -std=c++11 -o Solver -O3 Solver.cpp`

##How to


```
int main(int argc, char *argv[]) {
  togasat::Solver solver;
  std::string problem_name = argv[1];
  solver.parse_dimacs_problem(problem_name);
  togasat::lbool status = solver.solve();
  solver.print_answer();
}
```

`./Solver <cnf problem>`  

#####SATISFIABLE
If Sat Solver prove a given problem is SATISFIABLE(SAT),print SAT and model for problem.  
`SAT  
-1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 13 -14 15 16 0`

The model is seperated by space and ended with zero(0).

A postive integer represents True.  
A negative integer represents False.

For example,`-1` represents `X1=False`,`13` represents `X13=True`.

#####UNSATISFIABLE
only print UNSAT.  
`UNSAT`

####Algorithm
- DPLL
- Clause learning
- Two literal watching
- Back jump
- Phase saving

