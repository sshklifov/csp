#ifndef SOLVER_INCLUDED
#define SOLVER_INCLUDED

#include <vector>

const int MAX_VERTICES = 100;
const int USED_CAPACITY = 2*MAX_VERTICES;
const int SEED = 64;

class Solver
{
public:
    explicit Solver(int n);
    explicit Solver(const char* path);
    Solver(const Solver&) = delete;
    Solver(Solver&&) = delete;

    void Random(int n);
    void Print();

    void Reload(const char* path);
    bool Search();

private:
    void RootTree(int root);
    void NodeDepth(int u);
    void NodeHeight(int u);
    void NodeCount(int u);

private:
    void Order();
    void DfsOrder(); // TODO

    void PredictPrio();
    void BacktrackLoop(int u);
    void Backtrack(int u, int val);

private:
    bool CheckIsTree();
    bool CheckSolution();
    bool BacktrackInvariant(int u);

public:
    // if loaded
    int n;
    std::vector<int> graph[MAX_VERTICES];

    // if rooted
    int root;
    int pred[MAX_VERTICES];

    // if predicted
    int depth[MAX_VERTICES];
    int height[MAX_VERTICES];
    int nodeCount[MAX_VERTICES];
    float stability[MAX_VERTICES];

    // if ordered
    int nextVertex[MAX_VERTICES];

    // if ready
    int values[MAX_VERTICES];
    bool usedValues[USED_CAPACITY];
    bool usedDiff[USED_CAPACITY];
    /* int nextMin, nextMax; */

    enum {LOADED, ROOTED, PREDICT, ORDERED, READY} status;
};

#endif
