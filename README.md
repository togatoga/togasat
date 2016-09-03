# Togasat
CDCL SAT Solver for programming contest.  
Togasat is CDCL SAT Solver by modifying minisat.

##Install
`g++ -std=c++11 -o Solver -O3 Solver.cpp`

##How to
If you want to add a (x1 v x2 v not x3) clause,
```
togast::Solver solver;

std::vector<int> clause;
clause.push_back(1);//x1
clause.push_back(2);//x2
clause.push_back(-3);//not x3

solver.addClause(clause);//add (x1 v x2 v not x3)
```


```
solver.assigns[0] = 0;//X1 = True
solver.assigns[1] = 1;//X2 = False
solver.assigns[i] = 0;//X_{i + 1} = True
```

####How to solve cnf

```
int main(int argc, char *argv[]) {
  togasat::Solver solver;
  std::string problem_name = argv[1];
  solver.parse_dimacs_problem(problem_name);//parse problem
  togasat::lbool status = solver.solve();//solve sat problem
  solver.print_answer();//print answer
}
```

status is 0,SATISFIABLE  
status is 1,UNSATSFIABLE  
status is 2,UNKNOWN



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


##Tutorial for 8-queen problem



##Algorithm
- DPLL
- Clause learning
- Two literal watching
- Back jump
- Phase saving

