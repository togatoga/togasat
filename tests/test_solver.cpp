// Tests for Solver class

TEST_SUITE("Solver Basic Operations") {
  TEST_CASE("Empty solver construction") {
    togasat::Solver solver;
    CHECK(solver.assigns.size() == 0);
  }

  TEST_CASE("Solver answer starts as undefined") {
    togasat::Solver solver;
    std::vector<int> c1 = {1, 2};
    solver.addClause(c1);
    CHECK(solver.answer == togasat::l_Undef);
  }
}

TEST_SUITE("SAT Instances") {
  TEST_CASE("Simple SAT: (x1 OR x2) AND (NOT x1 OR x2)") {
    togasat::Solver solver;
    std::vector<int> c1 = {1, 2};
    std::vector<int> c2 = {-1, 2};
    solver.addClause(c1);
    solver.addClause(c2);
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
    CHECK(solver.answer == togasat::l_True);
  }

  TEST_CASE("SAT with unit propagation: (x1) AND (NOT x1 OR x2)") {
    togasat::Solver solver;
    std::vector<int> c1 = {1};
    std::vector<int> c2 = {-1, 2};
    solver.addClause(c1);
    solver.addClause(c2);
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
  }

  TEST_CASE("SAT: 3-clause satisfiable formula") {
    togasat::Solver solver;
    std::vector<int> c1 = {1, 2, 3};
    std::vector<int> c2 = {-1, 2};
    std::vector<int> c3 = {-2, 3};
    solver.addClause(c1);
    solver.addClause(c2);
    solver.addClause(c3);
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
  }
}

TEST_SUITE("UNSAT Instances") {
  TEST_CASE(
      "UNSAT: (x1 OR x2) AND (x1 OR NOT x2) AND (NOT x1 OR x2) AND (NOT x1 OR "
      "NOT x2)") {
    togasat::Solver solver;
    // All 4 combinations - impossible to satisfy
    std::vector<int> c1 = {1, 2};
    std::vector<int> c2 = {1, -2};
    std::vector<int> c3 = {-1, 2};
    std::vector<int> c4 = {-1, -2};
    solver.addClause(c1);
    solver.addClause(c2);
    solver.addClause(c3);
    solver.addClause(c4);
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_False);
    CHECK(solver.answer == togasat::l_False);
  }

  TEST_CASE("UNSAT: 3-variable all-negative") {
    togasat::Solver solver;
    // (x1 OR x2 OR x3) AND (NOT x1) AND (NOT x2) AND (NOT x3)
    std::vector<int> c1 = {1, 2, 3};
    std::vector<int> c2 = {-1};
    std::vector<int> c3 = {-2};
    std::vector<int> c4 = {-3};
    solver.addClause(c2);  // Add unit clauses first
    solver.addClause(c3);
    solver.addClause(c4);
    solver.addClause(c1);  // Then the clause that conflicts
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_False);
  }
}

TEST_SUITE("DIMACS Parsing") {
  TEST_CASE("Parse SAT problem from file") {
    togasat::Solver solver;
    solver.parseDimacsProblem("sample/sample_sat_problem.cnf");
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
  }

  TEST_CASE("Parse second sample problem from file") {
    // Note: sample_unsat_problem.cnf is actually SAT
    togasat::Solver solver;
    solver.parseDimacsProblem("sample/sample_unsat_problem.cnf");
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
  }
}

TEST_SUITE("Edge Cases") {
  TEST_CASE("Single variable SAT") {
    togasat::Solver solver;
    std::vector<int> c1 = {1};
    solver.addClause(c1);
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
    CHECK(solver.assigns[0] == togasat::l_True);
  }

  TEST_CASE("Duplicate unit clauses (Issue #9)") {
    togasat::Solver solver;
    std::vector<int> c1 = {1};
    std::vector<int> c2 = {1};
    solver.addClause(c1);
    solver.addClause(c2);  // same unit clause - should not crash
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
  }

  TEST_CASE("Conflicting unit clauses") {
    togasat::Solver solver;
    std::vector<int> c1 = {1};
    std::vector<int> c2 = {-1};
    solver.addClause(c1);
    solver.addClause(c2);  // conflicting unit clause
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_False);
  }

  TEST_CASE("Large clause") {
    togasat::Solver solver;
    std::vector<int> clause;
    for (int i = 1; i <= 100; ++i) {
      clause.push_back(i);
    }
    solver.addClause(clause);
    togasat::lbool result = solver.solve();
    CHECK(result == togasat::l_True);
  }

  TEST_CASE("Multiple independent solvers") {
    togasat::Solver solver1;
    togasat::Solver solver2;

    std::vector<int> c1 = {1};
    std::vector<int> c2 = {-1};

    solver1.addClause(c1);
    solver2.addClause(c2);

    CHECK(solver1.solve() == togasat::l_True);
    CHECK(solver2.solve() == togasat::l_True);

    // Verify assignments are independent
    CHECK(solver1.assigns[0] == togasat::l_True);
    CHECK(solver2.assigns[0] == togasat::l_False);
  }
}
