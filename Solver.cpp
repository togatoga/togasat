#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <list>
#include <queue>

#include <unordered_set>
#include <unordered_map>
// SAT Solver
// CDCL Solver
namespace Togasat {
using Var = int;
using CRef = int;
using lbool = int;
const CRef CRef_Undef = UINT32_MAX;
class Solver {

public:
  const lbool l_True = 0;
  const lbool l_False = 1;
  const lbool l_Undef = 2;

  const int var_Undef = -1;

  // Literal
  struct Lit {
    int x;
    inline bool operator==(Lit p) const { return x == p.x; }
    inline bool operator!=(Lit p) const { return x != p.x; }
    inline bool operator<(Lit p) const { return x < p.x; }

    inline Lit operator~() {
      Lit q;
      q.x = x ^ 1;
      return q;
    }
  };

  inline Lit mkLit(Var var, bool sign) {
    Lit p;
    p.x = var + var + sign;
    return p;
  };

  inline bool sign(Lit p) const { return p.x & 1; }
  inline int var(Lit p) const { return p.x >> 1; }

  inline int toInt(Var v) { return v; }
  inline int toInt(Lit p) { return p.x; }
  inline Lit toLit(int x) {
    Lit p;
    p.x = x;
    return p;
  }
  const Lit lit_Undef = {-2};
  const Lit lit_Error = {-1};
  // VarData
  struct VarData {
    CRef reason;
    int level;
  };
  inline VarData mkVarData(CRef cr, int l) {
    VarData d = {cr, l};
    return d;
  }

  // Watcher
  struct Watcher {
    CRef cref;
    Lit blocker;
    Watcher() {}
    Watcher(CRef cr, Lit p) : cref(cr), blocker(p) {}
    bool operator==(const Watcher &w) const { return cref == w.cref; }
    bool operator!=(const Watcher &w) const { return cref != w.cref; }
  };

  // Clause
  class Clause {
  public:
    struct {
      bool learnt;
      int size;
    } header;
    std::vector<Lit> data; //(x1 v x2 v not x3)
    Clause() {}
    Clause(const std::vector<Lit> &ps, bool learnt) {
      header.learnt = learnt;
      header.size = ps.size();
      for (int i = 0; i < ps.size(); i++) {
        data.push_back(ps[i]);
      }
    }

    int size() const { return header.size; }
    bool learnt() const { return header.learnt; }
    Lit &operator[](int i) { return data[i]; }
    Lit operator[](int i) const { return data[i]; }
  };

  CRef alloc_clause(const std::vector<Lit> &ps, bool learnt = false) {
    static CRef res = 0;
    ca[res] = Clause(ps, learnt);
    return res++;
  }

  Var newVar(bool sign = true, bool dvar = true) {
    int v = nVars();

    assigns.push_back(l_Undef);
    vardata.push_back(mkVarData(CRef_Undef, 0));

    seen.push_back(false);
    polarity.push_back(sign);
    decision.push_back(0);
    // trail.push_back(Lit());
    setDecisionVar(v, dvar);
    return v;
  }

  bool addClause_(std::vector<Lit> &ps) {
    std::sort(ps.begin(), ps.end());
    // empty clause
    if (ps.size() == 0) {
      return false;
    } else {
      CRef cr = alloc_clause(ps, false);
      clauses.insert(cr);
      attachClause(cr);
    }

    return true;
  }
  void attachClause(CRef cr) {
    const Clause &c = ca[cr];
    assert(c.size() > 1);

    watches[~c[0].x].push_back(Watcher(cr, c[1]));
    watches[~c[1].x].push_back(Watcher(cr, c[0]));
  }

  // Input
  void readClause(const std::string &line, std::vector<Lit> &lits) {
    lits.clear();

    int parsed_lit, var;
    parsed_lit = var = 0;
    bool neg = false;
    for (int i = 0; i < line.size(); i++) {
      if (line[i] == '-') {
        neg = true;
      } else if (line[i] >= '0' and line[i] <= '9') {
        parsed_lit = 10 * parsed_lit + (line[i] - '0');
      } else {
        if (parsed_lit == 0)
          break;
        if (neg) {
          parsed_lit *= -1;
        }
        var = abs(parsed_lit) - 1;
        while (var >= nVars())
          newVar();
        lits.push_back(neg == false ? mkLit(var, false) : mkLit(var, true));

        parsed_lit = 0;
        neg = false;
      }
    }
  }
  void parse_dimacs_problem(std::string problem_name) {
    std::vector<Lit> lits;
    int vars = 0;
    int clauses = 0;
    std::string line;
    std::ifstream ifs(problem_name, std::ios_base::in);

    while (ifs.good()) {
      getline(ifs, line);
      if (line.size() > 0) {
        if (line[0] == 'p') {
          sscanf(line.c_str(), "p cnf %d %d", &vars, &clauses);
        } else if (line[0] == 'c' or line[0] == 'p') {
          continue;
        } else {
          readClause(line, lits);
          addClause_(lits);
        }
      }
    }
    ifs.close();
  }

  std::unordered_map<CRef, Clause> ca; // store clauses
  std::unordered_set<CRef> clauses;    // original problem;

  std::unordered_map<int, std::list<Watcher>> watches;
  std::vector<VarData> vardata; // store reason and level for each variable
  std::vector<lbool> assigns;   // The current assignments
  std::vector<bool> polarity;   // The preferred polarity of each variable
  std::vector<bool> decision;
  std::vector<bool> seen;
  // Todo
  std::vector<Lit> trail;
  std::vector<int> trail_lim;
  // Todo rename(not heap)
  std::queue<Var> order_heap;

  std::vector<Lit> model;
  std::vector<Lit> conflict;

  int nVars() const { return vardata.size(); }
  int decisionLevel() const { return trail_lim.size(); }
  lbool value(Lit p) const { return assigns[var(p)] ^ sign(p); }
  void setDecisionVar(Var v, bool b) {
    decision[v] = b;
    order_heap.push(v);
  }
  void uncheckedEnqueue(Lit p, CRef from = CRef_Undef) {
    assert(value(p) == l_Undef);
    assigns[var(p)] = not sign(p);
    vardata[var(p)] = mkVarData(from, decisionLevel());
    trail.push_back(p);
  }
  CRef propagate() {
    CRef confl = CRef_Undef;
    int qhead = 0;
    while (qhead < trail.size()) {
      Lit p = trail[qhead++];
      std::list<Watcher> &ws = watches[p.x];
      std::list<Watcher>::iterator i, j, end;
      i = j = ws.begin();
      end = ws.end();
      for (; i != end;) {
        Lit blocker = i->blocker;
        if (value(blocker) == l_True) {
          j = i++;
          j++;
          continue;
        }

        CRef cr = i->cref;
        Clause &c = ca[cr];
        Lit false_lit = ~p;
        if (c[0] == false_lit) {
          c[0] = c[1];
          c[1] = false_lit;
        }
        i++;

        Lit first = c[0];
        Watcher w = Watcher(cr, first);
      }
    }
    return confl;
  }
  lbool search() {
    int backtrack_level;
    std::vector<Lit> learnt_clause;
    while (true) {
      CRef confl = propagate();
    }
  };

  lbool solve() {
    model.clear();
    conflict.clear();
    lbool status = l_Undef;
    std::cerr << "---start---" << std::endl;
    while (status == l_Undef) {
      status = search();
    }
    return status;
  };
};
}

int main() {
  Togasat::Solver solver;
  std::string problem_name = "sample_problem.cnf";
  solver.parse_dimacs_problem(problem_name);
  solver.solve();
}
