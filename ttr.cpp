#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <vector>
#include <queue>
#include <assert.h>

using namespace std;
struct consumer;

struct goal{
    vector<consumer *>  dependents;
};

struct solution {};
struct generator {
    queue<goal *> goals; // to be solved.
};

template<typename T>
struct table {
    bool contains(T s);
    void add(T s);
};
struct consumer {
    goal *ancestor; // thing we are trying to solve.
    queue<goal *> goals; // to be solved to solve ancestor
    table<solution> tblsol;
};


bool resolve(goal *g, solution s) { return false; }
// consumer extracts a solution for its ancestor goal
void extractSoln(consumer *cur, solution s) {};


using genstack = stack<generator>;
using resumestack = stack<pair<consumer*, solution>>;

template<typename T>
T pop(stack<T> &s) {
    T t = s.top();
    s.pop();
    return t;
}
struct Ttr {
    optional<solution> solve(goal *g) {
        generator gnode;
        gnode.goals.push(g);
        gs.push(gnode);
        return ttrmain();
    }

    private:
    optional<solution> ttrmain() {
        while (1) {
            if (!rs.empty()) {                   
                consumer *cnode; solution s;
                tie(cnode, s) = pop(rs);
                assert(cnode->goals.size() > 0);
                if (!resolve(cnode->goals.front(), s)) { continue; }
                if (cnode->goals.size() == 1) {
                    extractSoln(cnode, s);
                    goal *g = cnode->ancestor;
                    if (g == original) { return {s}; }
                    if (cnode->tblsol.contains(s)) { continue; }
                    cnode->tblsol.add(s);
                    // push all other consumers.
                    for(consumer *c : g->dependents) {
                        if (c == cnode) { continue; }
                        rs.push({c, s});
                    }
                }
                else { 
                    // cnode has more goals
                    cnode->goals.pop();
                    newConsumerNode(cnode->goals);
                }
            } else if (!gs.empty()) {
                generator gnode = gs.top();
                (void)gnode;
                // if there are no remaining instances, then pop.
                if (/*no remaining instances */ false) {
                    gs.pop();
                } else if (/*next instance resolves with gnode's goal*/ false) {
                    // newConsumerNode(/*new subgoals */);
                }

            } else {
                fail();
            }
        }
    }

    consumer *newConsumerNode(queue<goal *> &gs) {
        assert(gs.size() > 0);
        goal *g = gs.front();
        if (!tblgoal.contains(g)) {
            tblgoal.add(g);
            gs.push(g);
        } 

        // for each existing solution to goal g, push consumer node along with
        // solution to the resume stack.
        // TODO: I don't get this.

        // Register the fact that the new consumer node (?) depends on its
        // first subgoal.
        consumer *c = new consumer;
        c->ancestor = g;
        g->dependents.push_back(c);
        return c;
    }


    optional<solution> fail() { return {}; };


    goal *original;
    table<goal *> tblgoal;
    genstack gs;
    resumestack rs;
};

int main() { return 0; }
