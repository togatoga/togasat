// Benchmark tests using standard SAT problem formats from SATLIB

TEST_SUITE("Benchmark SAT") {
  TEST_CASE("uf20-01.cnf - Uniform Random 3-SAT") {
    togasat::Solver solver;
    solver.parseDimacsProblem("benchmarks/uf20-01.cnf");
    CHECK(solver.solve() == togasat::l_True);
  }

  TEST_CASE("uf20-02.cnf - Uniform Random 3-SAT") {
    togasat::Solver solver;
    solver.parseDimacsProblem("benchmarks/uf20-02.cnf");
    CHECK(solver.solve() == togasat::l_True);
  }

  TEST_CASE("aim-50-1_6-yes1-1.cnf - AIM SAT") {
    togasat::Solver solver;
    solver.parseDimacsProblem("benchmarks/aim-50-1_6-yes1-1.cnf");
    CHECK(solver.solve() == togasat::l_True);
  }

  TEST_CASE("aim-50-1_6-yes1-2.cnf - AIM SAT") {
    togasat::Solver solver;
    solver.parseDimacsProblem("benchmarks/aim-50-1_6-yes1-2.cnf");
    CHECK(solver.solve() == togasat::l_True);
  }
}

TEST_SUITE("Benchmark UNSAT") {
  TEST_CASE("aim-50-1_6-no-1.cnf - AIM UNSAT") {
    togasat::Solver solver;
    solver.parseDimacsProblem("benchmarks/aim-50-1_6-no-1.cnf");
    CHECK(solver.solve() == togasat::l_False);
  }

  TEST_CASE("aim-50-1_6-no-2.cnf - AIM UNSAT") {
    togasat::Solver solver;
    solver.parseDimacsProblem("benchmarks/aim-50-1_6-no-2.cnf");
    CHECK(solver.solve() == togasat::l_False);
  }
}
