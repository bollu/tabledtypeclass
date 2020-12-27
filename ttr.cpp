#include <assert.h>

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

#include "parser.h"


extern "C" {
const char *__asan_default_options() { return "detect_leaks=0"; }
}

using namespace std;
struct Consumer;


// === PARSER ===
// === PARSER ===
// === PARSER ===
// === PARSER ===
// === PARSER ===
// === PARSER ===

// T(t1, t2, ... tn)
struct SurfaceType {
  Span span;
  SurfaceType(Span span) : span(span){};

  virtual void print(std::ostream &o) const = 0;
};

struct SurfaceTuple : public SurfaceType {
  vector<SurfaceType *> args;
  SurfaceTuple(Span span, vector<SurfaceType *> args)
      : SurfaceType(span), args(args){};

  void print(std::ostream &o) const override {
    o << "(";
    for (int i = 0; i < args.size(); ++i) {
      args[i]->print(o);
      o << ","[i + 1 == args.size()];
    }
    o << ")";
  }
};

struct SurfaceConst : public SurfaceType {
  Identifier name;
  SurfaceConst(Identifier name) : SurfaceType(name.span), name(name) {
    assert(!islower(name.name[0]) && "constants don't start with lowercase");
  };

  void print(std::ostream &o) const override { o << name; }
};

struct SurfaceVar : public SurfaceType {
  Identifier name;
  SurfaceVar(Identifier name) : SurfaceType(name.span), name(name) {
    assert(islower(name.name[0]) && "type variables start with lowercase");
  };

  void print(std::ostream &o) const override { o << name; }
};

// known typeclass inference rule of the form
// ("Eq", a) and ("Ord", a) => ("Ord", ("List", a))
struct SurfaceInstance {
  Span span;
  vector<SurfaceType *> lhs;
  SurfaceType *rhs;

  SurfaceInstance(Span span, vector<SurfaceType *> lhs, SurfaceType *rhs)
      : span(span), lhs(lhs), rhs(rhs) {}

  void print(std::ostream &o) const {
    o << "instance ";
    for (int i = 0; i < lhs.size(); ++i) {
      lhs[i]->print(o);
      if (i + 1 < lhs.size()) { o << " and "; }
    }

    o << " => ";
    rhs->print(o);
    o << ";";
  }
};

struct Module {
  vector<SurfaceInstance *> instances;
  // demand an instantiation. eg. Demand (Show (List(Char)) which will
  // be instantiated from Show(a) => Show(List(a)) /\ Show(Char)
  vector<SurfaceType *> demands;

  void print(std::ostream &o) {
    for (SurfaceInstance *i : instances) {
      i->print(o);
      o << "\n";
    }

    for (SurfaceType *t : demands) {
      o << "demand ";
      t->print(o);
      o << ";";
      o << "\n";
    }
  }
};

struct ModuleParser {
  static Module parse(Parser &p) {
    Module m;

    while (!p.eof()) {
      Identifier kind = p.parseIdentifier();
      if (kind.name == "instance") {
        vector<SurfaceType *> lhs;
        while (1) {
          lhs.push_back(parseSurfaceType(p));
          if (p.parseOptionalSigil("=>")) {
            break;
          } else if (p.parseOptionalIdentifier("and")) {
            continue;
          } else {
            p.addErrAtCurrentLoc("expected 'and' or '=>'");
          }
        } // end loop of LHS parsing
        SurfaceType *rhs = parseSurfaceType(p);
        p.parseSigil(";");
        m.instances.push_back(new SurfaceInstance(
            kind.span.extendRight(p.getCurrentLoc()), lhs, rhs));
      } else if (kind.name == "demand") {
        m.demands.push_back(parseSurfaceType(p));
        p.parseSigil(";");

      } else {
        p.addErr(ParseError(kind.span, "unknown declaration. expected "
                                       "'instance' or 'demand'"));
      } // end if/else/for instance/demand/err
    }   // end EOF loop
    return m;
  } // end parse.

  // ( <types> ) | Identifier
  static SurfaceType *parseSurfaceType(Parser &p) {
    if (optional<Span> open = p.parseOptionalSigil("(")) {
      if (optional<Span> close = p.parseOptionalSigil(")")) {
        return new SurfaceTuple(open->extendRight(*close), {});
      }
      // we have "(" and one or more arguments.
      vector<SurfaceType *> args;
      while (1) {
        args.push_back(parseSurfaceType(p));

        if (p.parseOptionalComma()) {
          continue;
        } else {
          p.parseCloseRoundBracket(*open);
          break;
        }
      }
      return new SurfaceTuple(open->extendRight(p.getCurrentLoc()), args);
    } else {
      Identifier name = p.parseIdentifier();
      if (islower(name.name[0])) {
        return new SurfaceVar(name);
      } else {
        return new SurfaceConst(name);
      }
    }
  }
};

// === SOLVER ===
// === SOLVER ===
// === SOLVER ===
// === SOLVER ===
// === SOLVER ===
// === SOLVER ===

// label the variables as 0, 1, 2, ...
SurfaceType *canonicalize(SurfaceType *t) {
    return t;
}


struct Goal {
  SurfaceType *tosolve;
  vector<Consumer *> dependents;

  Goal(SurfaceType *tosolve) : tosolve(tosolve) {};
};

struct Solution {
    void print(ostream &cerr) const {
        cerr << "solution";
    }
};

struct Generator {
  queue<Goal *> goals; // to be solved.
};

template <typename T> struct table {
  bool contains(T s);
  void add(T s);
};
struct Consumer {
  Goal *ancestor;      // thing we are trying to solve.
  queue<Goal *> goals; // to be solved to solve ancestor
  table<Solution *> tblsol;
};

// resolve a goal with a solution??
bool resolve(Goal *g, Solution *&s) { return false; }
// Consumer extracts a solution for its ancestor Goal
void extractSoln(Consumer *cur, Solution *s){};

using genstack = stack<Generator>;
using resumestack = stack<pair<Consumer *, Solution*>>;

template <typename T> T pop(stack<T> &s) {
  T t = s.top();
  s.pop();
  return t;
}
struct Solver {

    vector<SurfaceInstance *>is;
    Solver(vector<SurfaceInstance *> is) : is(is) {};

    optional<Solution *> solve(SurfaceType *g) {
        Generator gnode;
        gnode.goals.push(new Goal(g));
        gs.push(gnode);
        return ttrmain();
  }

private:
  optional<Solution *> ttrmain() {
    while (1) {
      if (!rs.empty()) {
        Consumer *cnode;
        Solution *s = nullptr;
        tie(cnode, s) = pop(rs);
        assert(cnode->goals.size() > 0);
        if (!resolve(cnode->goals.front(), s)) {
          continue;
        }
        if (cnode->goals.size() == 1) {
          extractSoln(cnode, s);
          Goal *g = cnode->ancestor;
          if (g == original) {
            return {s};
          }
          if (cnode->tblsol.contains(s)) {
            continue;
          }
          cnode->tblsol.add(s);
          // push all other consumers.
          for (Consumer *c : g->dependents) {
            if (c == cnode) {
              continue;
            }
            rs.push({c, s});
          }
        } else {
          // cnode has more goals
          cnode->goals.pop();
          newConsumerNode(cnode->goals);
        }
      } else if (!gs.empty()) {
        Generator gnode = gs.top();
        (void)gnode;
        // if there are no remaining instances, then pop.
        if (/*no remaining instances */ false) {
          gs.pop();
        } else if (/*next instance resolves with gnode's Goal*/ false) {
          // newConsumerNode(/*new subgoals */);
        }

      } else {
        fail();
      }
    }
  }

  Consumer *newConsumerNode(queue<Goal *> &gs) {
    assert(gs.size() > 0);
    Goal *g = gs.front();
    if (!tblgoal.contains(g)) {
      tblgoal.add(g);
      gs.push(g);
    }

    // for each existing solution to Goal g, push Consumer node along with
    // solution to the resume stack.
    // TODO: I don't get this.

    // Register the fact that the new Consumer node (?) depends on its
    // first subgoal.
    Consumer *c = new Consumer;
    c->ancestor = g;
    g->dependents.push_back(c);
    return c;
  }

  optional<Solution *> fail() { return {}; };

  Goal *original;
  table<Goal *> tblgoal;
  genstack gs;
  resumestack rs;
};



// === ENTRY ===
// === ENTRY ===
// === ENTRY ===
// === ENTRY ===
// === ENTRY ===
// === ENTRY ===

static const int MAXFILESIZE = 1e9;
char buffer[MAXFILESIZE];
int main(int argc, char **argv) {
  assert(argc == 2 && "usage: ttr <path-to-source>");

  FILE *f = fopen(argv[1], "r");
  assert (f && "unable to open source file");
  fread(buffer, 1, MAXFILESIZE, f);
  fclose(f);

  Parser p(argv[1], buffer);
  ModuleParser pm;
  Module m = pm.parse(p);
  cerr << "===module:===\n";
  m.print(cerr);

  Solver solver(m.instances);
  for(SurfaceType *t : m.demands) {
      cerr << "---solving |";
      t->print(cerr); 
      cerr <<  "|---\n";
      std::optional<Solution *> soln = solver.solve(t);
      if (!soln) { cerr << "NO SOLUTION\n"; }
      else {
          (*soln)->print(cerr);
      }
  }
  cerr << "===\n";
  return 0;
}
