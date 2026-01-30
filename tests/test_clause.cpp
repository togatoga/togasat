// Tests for Clause functionality through public API
// Note: Clause class is private, tested via addClause

TEST_SUITE("Clause via Public API") {
  TEST_CASE("Adding single clause") {
    togasat::Solver solver;
    std::vector<int> clause = {1, 2, 3};
    solver.addClause(clause);
    CHECK(solver.assigns.size() == 3);
  }

  TEST_CASE("Adding unit clause") {
    togasat::Solver solver;
    std::vector<int> clause = {1};
    solver.addClause(clause);
    CHECK(solver.assigns.size() == 1);
  }

  TEST_CASE("Adding clause with negated literals") {
    togasat::Solver solver;
    std::vector<int> clause = {1, -2, 3};
    solver.addClause(clause);
    CHECK(solver.assigns.size() == 3);
  }

  TEST_CASE("Adding multiple clauses") {
    togasat::Solver solver;
    std::vector<int> c1 = {1, 2};
    std::vector<int> c2 = {-1, 3};
    std::vector<int> c3 = {-2, -3};
    solver.addClause(c1);
    solver.addClause(c2);
    solver.addClause(c3);
    CHECK(solver.assigns.size() == 3);
  }
}
