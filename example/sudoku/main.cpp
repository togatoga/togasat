#include <cassert>
#include <iostream>
#include <vector>

#include "../../togasat.hpp"

// Variable encoding: variable(row, col, digit) = 81*row + 9*col + digit
// where row, col in [0,8], digit in [1,9]
// Returns 1-indexed variable for SAT solver
int variable(int row, int col, int digit) {
  assert(row >= 0 && row < 9);
  assert(col >= 0 && col < 9);
  assert(digit >= 1 && digit <= 9);
  return 81 * row + 9 * col + digit;
}

int main() {
  togasat::Solver solver;
  int height, width;
  std::cin >> height >> width;
  assert(height == 9 && width == 9);

  // Read the puzzle and add unit clauses for given numbers
  std::vector<std::vector<int>> puzzle(9, std::vector<int>(9, 0));
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      std::string text;
      std::cin >> text;
      if (text != "-") {
        int digit = text[0] - '0';
        puzzle[i][j] = digit;
        std::vector<int> clause = {variable(i, j, digit)};
        solver.addClause(clause);
      }
    }
  }

  // Constraint 1: Each cell has at least one digit
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      std::vector<int> clause;
      for (int digit = 1; digit <= 9; digit++) {
        clause.push_back(variable(row, col, digit));
      }
      solver.addClause(clause);
    }
  }

  // Constraint 2: Each cell has at most one digit (at-most-one)
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      for (int d1 = 1; d1 <= 9; d1++) {
        for (int d2 = d1 + 1; d2 <= 9; d2++) {
          std::vector<int> clause = {-variable(row, col, d1),
                                     -variable(row, col, d2)};
          solver.addClause(clause);
        }
      }
    }
  }

  // Constraint 3: Each row has all digits
  for (int row = 0; row < 9; row++) {
    for (int digit = 1; digit <= 9; digit++) {
      std::vector<int> clause;
      for (int col = 0; col < 9; col++) {
        clause.push_back(variable(row, col, digit));
      }
      solver.addClause(clause);
    }
  }

  // Constraint 4: Each column has all digits
  for (int col = 0; col < 9; col++) {
    for (int digit = 1; digit <= 9; digit++) {
      std::vector<int> clause;
      for (int row = 0; row < 9; row++) {
        clause.push_back(variable(row, col, digit));
      }
      solver.addClause(clause);
    }
  }

  // Constraint 5: Each 3x3 box has all digits
  for (int box_row = 0; box_row < 3; box_row++) {
    for (int box_col = 0; box_col < 3; box_col++) {
      for (int digit = 1; digit <= 9; digit++) {
        std::vector<int> clause;
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 3; j++) {
            int row = 3 * box_row + i;
            int col = 3 * box_col + j;
            clause.push_back(variable(row, col, digit));
          }
        }
        solver.addClause(clause);
      }
    }
  }

  // Solve
  togasat::lbool result = solver.solve();

  if (result == togasat::l_True) {
    std::cout << "Solution found:" << std::endl;
    for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
        for (int digit = 1; digit <= 9; digit++) {
          int var = variable(row, col, digit) - 1;  // 0-indexed for assigns
          if (solver.assigns[var] == togasat::l_True) {
            std::cout << digit;
            if (col < 8) std::cout << " ";
            break;
          }
        }
      }
      std::cout << std::endl;
    }
  } else {
    std::cout << "No solution exists." << std::endl;
  }

  return 0;
}
