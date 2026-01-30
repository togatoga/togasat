# togasat

[![CI](https://github.com/togatoga/togasat/actions/workflows/ci.yml/badge.svg)](https://github.com/togatoga/togasat/actions/workflows/ci.yml)

togasat, based on MiniSat, is a header-only CDCL SAT Solver for programming contests.

## Minimum requirement C++ version
- C++11

## How to use togasat as a library

At first, include the header (or do copy & paste):

``` c++
#include "togasat.hpp"
```

Make a solver object.

``` c++
   togasat::Solver solver;
```

If you want to add a (x1 v x2 v not x3) clause

``` c++
    std::vector<int> clause;
    clause.push_back(1);  // x1
    clause.push_back(2);  // x2
    clause.push_back(-3);  // not x3

    solver.addClause(clause);  // add (x1 v x2 v not x3)
```
**YOU MUST NOT ADD CLAUSES THAT CONTAIN 0, IT WILL CAUSE A SAT SOLVER ABORT**

After adding all clauses, call `.solve()` method.

``` c++
    togasat::lbool status = solver.solve();
```

The return value (namespace-level constants):

-   `togasat::l_True` (0): `SATISFIABLE`
-   `togasat::l_False` (1): `UNSATISFIABLE`
-   `togasat::l_Undef` (2): `UNKNOWN`

Also, you can get the assignments, e.g.

``` c++
   solver.assigns[0] == togasat::l_True;   // X_1 = True
   solver.assigns[1] == togasat::l_False;  // X_2 = False
   solver.assigns[i] == togasat::l_True;   // X_{i + 1} = True
```

You should take care that `l_True` (0) denotes `true` and `l_False` (1) denotes `false`.

## How to solve CNF

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

If Sat Solver proves a given problem is SATISFIABLE(SAT), it prints SAT and a model for the problem.

```
SAT
-1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 13 -14 15 16 0
```

The model is separated by a space and ended with zero(0).

-   A positive integer represents `True`.
-   A negative integer represents `False`.

For example,`-1` represents `X1=False`,`13` represents `X13=True`.

### UNSATISFIABLE

It only prints UNSAT.

```
UNSAT
```

## Example: Sudoku Solver

The `example/sudoku/` directory contains a complete Sudoku solver using togasat:

```bash
cd example/sudoku
g++ -std=c++11 -O2 main.cpp -o sudoku
./sudoku < problem.txt
```

## Running Tests

Tests use the [doctest](https://github.com/doctest/doctest) framework:

```bash
make test
```

## Algorithms

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

## Contributors
- [mugenen](https://github.com/mugenen)
- [kmyk](https://github.com/kmyk)
- [koba-e964](https://github.com/koba-e964)

## Note
Welcome any pull requests, fix of typo, refactoring, correction of documents and so on.
If you can solve a competitive programming problem by togasat, please let me know and you make a pull request.
It's very helpful for me.

**Your star makes me motivated**
