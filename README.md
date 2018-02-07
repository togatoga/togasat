# togasat

togasat based on minisat is CDCL SAT Solver for programming contest.

## Install

```
$ g++ -std=c++11 -o Solver -O3 Solver.cpp
````

## How to use as a library

At first, include the header (or do copy & paste):

``` c++
#include "Solver.hpp"
```

Make a solver object.

``` c++
   togast::Solver solver;
```

If you want to add a (x1 v x2 v not x3) clause

``` c++
    std::vector<int> clause;
    clause.push_back(1);  // x1
    clause.push_back(2);  // x2
    clause.push_back(-3);  // not x3

    solver.addClause(clause);  // add (x1 v x2 v not x3)
```
**YOU MUST NOT ADD CLAUSES CONTAINS ZERO VALUE, SAT SOLVER IS ABORETED**

After adding all clauses, call `.solve()` method.

``` c++
    togasat::lbool status = solver.solve();
```

The return value:

-   status is 0, SATISFIABLE
-   status is 1, UNSATSFIABLE
-   status is 2, UNKNOWN

Also, you can get the assignments, e.g.

``` c++
   solver.assigns[0] = 0;  // X_1 = True
   solver.assigns[1] = 1;  // X_2 = False
   solver.assigns[i] = 0;  // X_{i + 1} = True
```

You should take care that `0` denotes `true` and `1` denotes `false`.

## How to solve cnf

```
$ ./Solver <cnf problem>
```


``` c++
int main(int argc, char *argv[]) {
    togasat::Solver solver;
    std::string problem_name = argv[1];
    solver.parseDimacsProblem(problem_name);  // parse problem
    togasat::lbool status = solver.solve();  // solve sat problem
    solver.printAnswer();  // print answer
}
```

### SATISFIABLE

If Sat Solver prove a given problem is SATISFIABLE(SAT),print SAT and model for problem.

```
SAT
-1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 13 -14 15 16 0
```

The model is seperated by space and ended with zero(0).

-   A positive integer represents `True`.
-   A negative integer represents `False`.

For example,`-1` represents `X1=False`,`13` represents `X13=True`.

### UNSATISFIABLE

only print UNSAT.

```
UNSAT
```

## Algorithm

-   DPLL
-   Clause learning
-   Two literal watching
-   Back jump
-   Phase saving
-   Luby restart
-   VSIDS

## Problems solved by togasat
### [Codeforces](http://codeforces.com)
- [Codeforces Round #383 (Div. 2) E](http://codeforces.com/contest/742/submission/22846987)

## Contributers
- [mugenen](https://github.com/mugenen)
- [kmyk](https://github.com/kmyk)

## Note
Welcome any pull requests, fix typo, refactor, correct document and so on.
If you can solve a competitive programming problem by togasat, please let me know and you make a pull request.
It's very helpful for me.
