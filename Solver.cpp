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

  // lifted boolean
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
    } else if (ps.size() == 1){
      uncheckedEnqueue(ps[0]);
    }else{
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
        while (var >= nVars()) {
          newVar();
        }
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
  std::unordered_set<CRef> learnts;

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
  void newDecisionLevel() { trail_lim.push_back(trail.size()); }

  inline CRef reason(Var x) const { return vardata[x].reason; }
  inline int level(Var x) const { return vardata[x].level; }
  bool satisfied(const Clause &c) const {
    for (int i = 0; i < c.size(); i++) {
      if (value(c[i]) == l_True) {
        return true;
      }
    }
    return false;
  }
  lbool value(Var p) const { return assigns[p]; }
  lbool value(Lit p) const {
    if (assigns[var(p)] == l_Undef) {
      return l_Undef;
    }
    return assigns[var(p)] ^ sign(p);
  }
  void setDecisionVar(Var v, bool b) {
    decision[v] = b;
    order_heap.push(v);
  }
  void uncheckedEnqueue(Lit p, CRef from = CRef_Undef) {
    // std::cout << p.x << " " << value(p) << std::endl;
    // std::cout << p.x << " " << value(p) << " " << std::endl;

    // if (value(p) != l_Undef)return;
    //std::cout << "decision = " << p.x << " " << from << " " << decisionLevel() << std::endl;
    assert(value(p) == l_Undef);
    assigns[var(p)] = sign(p);
    vardata[var(p)] = mkVarData(from, decisionLevel());
    trail.push_back(p);
  }
  // decision
  Lit pickBranchLit() {
    Var next = var_Undef;
    while (next == var_Undef or value(next) != l_Undef) {
      if (order_heap.empty()) {
        next = var_Undef;
        break;
      } else {
        next = order_heap.front();
        order_heap.pop();
      }
    }
    //    std::cout << "pick!!! " << next << " " << value(next) << std::endl;
    return next == var_Undef ? lit_Undef : mkLit(next, polarity[next]);
  }

  // clause learning
  void analyze(CRef confl, std::vector<Lit> &out_learnt, int &out_btlevel) {
    int pathC = 0;
    Lit p = lit_Undef;
    int index = trail.size() - 1;
    // std::cout << seen.size() << std::endl;
    // std::cout << __LINE__ << std::endl;
    //std::cout << "DecisonLevel = " << decisionLevel() << std::endl;
    out_learnt.push_back(mkLit(0, false));
    do {
      //std::cout << "conflict = " << confl << std::endl;
      assert(confl != CRef_Undef);

      Clause &c = ca[confl];
      // std::cout << pathC << std::endl;
      for (int j = (p == lit_Undef) ? 0 : 1; j < c.size(); j++) {
	
        Lit q = c[j];
        // std::cout << j << " " << q.x << std::endl;

	//std::cout << "lit = "<< q.x << " level = " << level(var(q)) << " seen = " << seen[var(q)] << std::endl;
        if (not seen[var(q)] and level(var(q)) > 0) {
          seen[var(q)] = 1;
          if (level(var(q)) >= decisionLevel()) {
            pathC++;
          } else {
            out_learnt.push_back(q);
          }
        }
      }
      
      while (not seen[var(trail[index--])]);

      
      p = trail[index + 1];
      confl = reason(var(p));
      seen[var(p)] = 0;
      pathC--;
      //std::cout << p.x << " " << confl << " " << pathC << std::endl;
      // std::cout << confl << std::endl;
    } while (pathC > 0);

    out_learnt[0] = ~p;

    //
    if (out_learnt.size() == 1) {
      out_btlevel = 0;
    } else {
      int max_i = 1;
      for (int i = 2; i < out_learnt.size(); i++) {
        if (level(var(out_learnt[i])) > level(var(out_learnt[max_i]))) {
          max_i = i;
        }
      }

      Lit p = out_learnt[max_i];
      out_learnt[max_i] = out_learnt[1];
      out_learnt[1] = p;
      out_btlevel = level(var(p));
    }

    for (int i = 0; i < out_learnt.size(); i++) {
      seen[var(out_learnt[i])] = false;
    }
    
  }

  // backtrack
  void cancelUntil(int level) {
    // std::cout << decisionLevel() << " " << level << std::endl;
    if (decisionLevel() > level) {
      for (int c = trail.size() - 1; c >= trail_lim[level]; c--) {
        Var x = var(trail[c]);
        assigns[x] = l_Undef;
        polarity[x] = sign(trail[c]);
        order_heap.push(x);
      }
      trail.erase(trail.end() - (trail.size() - trail_lim[level]), trail.end());
      trail_lim.erase(trail_lim.end() - (trail_lim.size() - level),
                      trail_lim.end());
    }
    // std::cout << trail.size() << " " << trail_lim.size() << std::endl;
  }

  // navie check sat
  // replace propagate
  CRef naive_check_sat() {
    CRef confl = CRef_Undef;
    int cnt = 1;

      
    while (cnt > 0){
      cnt--;
      for (const CRef &cr : clauses) {
	Clause &c = ca[cr];
	if (satisfied(c))
	  continue;
	int cnt_conflict = 0;
	Lit first;
	for (int i = 0; i < c.size(); i++) {
	  if (value(c[i]) == l_False) {
	    cnt_conflict++;
	  } else {
	    first = c[i];
	  }
	}
	if (cnt_conflict == c.size()) { // conflict
	  return cr;
	}
	if (cnt_conflict == c.size() - 1) {
	  //uncheckedEnqueue(first, cr);
	  //cnt++;
	}
      }
    }
    return confl;
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
    learnt_clause.push_back(mkLit(-1, false));
    while (true) {
      CRef confl = naive_check_sat();
      // std::cout << confl << " " << decisionLevel() << " " << trail.size() <<
      // std::endl;
      if (confl != CRef_Undef) {
        // CONFLICT
        if (decisionLevel() == 0)
          return l_False;
        learnt_clause.clear();
        analyze(confl, learnt_clause, backtrack_level);
        cancelUntil(backtrack_level);
	// std::cout << "learnt_clause.size() = " << learnt_clause.size() << std::endl;
	// std::cout << "---learnt---" << std::endl;
	// for (int i = 0; i < learnt_clause.size(); i++){
	//   std::cout << learnt_clause[i].x << " ";
	// }
	// std::cout << std::endl;
	// std::cout << "---learnt---" << std::endl;
        if (learnt_clause.size() == 1) {
          uncheckedEnqueue(learnt_clause[0]);
        } else {
          CRef cr = alloc_clause(learnt_clause, true);
          learnts.insert(cr);
          attachClause(cr);
          uncheckedEnqueue(learnt_clause[0], cr);
        }

      } else {
        // NO CONFLICT

        Lit next = pickBranchLit();

        if (next == lit_Undef) {
          return l_True;
        }
        newDecisionLevel();
        uncheckedEnqueue(next);
      }
    }
  };

  lbool solve() {
    model.clear();
    conflict.clear();
    lbool status = l_Undef;
    //std::cerr << "---start---" << std::endl;
    while (status == l_Undef) {
      status = search();
    }
    return status;
  };
  
  void addClause(std::vector<int> &clause){
    std::vector<Lit> lits;
    for (int i = 0; i < clause.size(); i++){
      int var = abs(clause[i]) - 1;
      while (var >= newVar());
      lits.push_back(var > 0 ? mkLit(var, false) : mkLit(var, true));
    }
    addClause_(lits);
  }

  
};
}

int main(int argc, char *argv[]) {
  Togasat::Solver solver;
  //std::string problem_name = "sample_unsat_problem.cnf";
  //std::string problem_name = "sample_sat_problem.cnf";
  std::string problem_name = argv[1];  
  solver.parse_dimacs_problem(problem_name);
  Togasat::lbool status = solver.solve();
  std::cout << status << std::endl;
}
