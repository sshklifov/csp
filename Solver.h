#ifndef SOLVER_INCLUDED
#define SOLVER_INCLUDED

#include <vector>

// The size of the input is heavily limited by the theoretical n! time
// Thus, it makes sense to use stack memory instead of heap.
const int MAX_VERTICES = 100;
const int USED_CAPACITY = 2 * MAX_VERTICES;
const int SEED = 611;

// Class which does the heavy lifting. It can be initialized with
// - a uniform random tree
// - by reading it from a file
// Then you can call Search which uses a backtracking algorithm
class Solver {
public:
    Solver();
    explicit Solver(int n);
    explicit Solver(const char* path);
    Solver(const Solver&) = delete;
    Solver(Solver&&) = delete;

    void Random(int n);

    void Reload(const char* path);
    void Search();

private: // Defined in SolverUtil.cpp
    void RootTree(int root);
    void NodeDepth(int u);
    void NodeHeight(int u);
    void NodeCount(int u);
    void Print();

private: // Defined in Solver.cpp
    void Order();
    void PredictPrio();
    void SearchWithAlarm();
    void BacktrackLoop(int u);
    void Backtrack(int u, int val);

private: // Defined in AssertFun.cpp
    // Validating functions. Used #ifndef DNDEBUG
    bool CheckIsTree();
    bool CheckSolution();
    bool BacktrackInvariant(int u);

public:
    // Values accessible if status >= LOADED
    int n;
    std::vector<int> graph[MAX_VERTICES];

    // Values accessible if status >= ROOTED
    int root;
    int pred[MAX_VERTICES];

    // Values accessible if status >= PREDICT
    int depth[MAX_VERTICES];
    int height[MAX_VERTICES];
    int nodeCount[MAX_VERTICES];
    float certainty[MAX_VERTICES];

    // Values accessible if status >= ORDERED
    int nextVertex[MAX_VERTICES];

    // Values used by the recursive solver
    // must be reset for each invocation of Backtrack
    int values[MAX_VERTICES];
    bool usedValues[USED_CAPACITY];
    bool usedDiff[USED_CAPACITY];
    int nextMin, nextMax;

    // Variable controlling flow
    enum { INVALID, LOADED, ROOTED, PREDICT, ORDERED } status;
};

#endif
