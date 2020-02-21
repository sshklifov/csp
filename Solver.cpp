// Used to execute getcertainty.py
#ifdef HAVE_PYTHON
    #define PY_SSIZE_T_CLEAN
    #include <Python.h>
#endif

#include "Solver.h"

#include <cstdio>
#include <cassert>
#include <queue>
#include <functional>
#include <algorithm>

Solver::Solver() : status(INVALID)
{
}

Solver::Solver(int n)
{
    // generates a uniform random tree, see SolverUtil.cpp
    Random(n);
}

Solver::Solver(const char* path)
{
    Reload(path);
}

void Solver::Reload(const char* path)
{
    FILE* fp = fopen(path, "r");
    assert(fp);

    fscanf(fp, "%d", &n);
    assert(n <= MAX_VERTICES);

    for (int i = 0; i < MAX_VERTICES; ++i) graph[i].clear();

    int v1, v2;
    while (fscanf(fp, "%d %d", &v1, &v2) == 2) {
        graph[v1].push_back(v2);
        graph[v2].push_back(v1);
    }

    assert(!ferror(fp));
    status = LOADED;
    assert(CheckIsTree());
}

// Create a sinnal handler for SIGALRM
#ifdef MAX_SECONDS
    #include <csignal>
    #include <unistd.h>
sig_atomic_t loop;
static void SignalHandler(int)
{
    loop = false;
}
#endif

void Solver::SearchWithAlarm()
{
    assert(status == LOADED);

    // Consider every node as the root. Prioritize nodes with higher degrees
    std::vector<int> roots(n);
    for (int i = 0; i < n; ++i) roots[i] = i;
    std::sort(roots.begin(), roots.end(), [this](int a, int b) {
        return graph[a].size() > graph[b].size();
    });

    for (int root : roots) {
        RootTree(root);
        assert(status == ROOTED);
        Order();
        assert(status == ORDERED);

        // Clear backtrack state variables
        for (int& x : values) x = -1;
        for (bool& x : usedValues) x = false;
        for (bool& x : usedDiff) x = false;

        // Stores the next smallest/biggest available value, see BacktrackLoop
        nextMin = 3;
        nextMax = 2 * n - 1;

        values[root] = 1;
        usedValues[1] = true;

        try {
            BacktrackLoop(nextVertex[root]);
#ifdef MAX_SECONDS
            if (!loop) // loop==false iff MAX_SECONDS have passed
            {
                printf("Timed out!\n");
                status = INVALID;
                return;
            }
#endif
        }
        catch (int) // if a solution is found, throw an int to unwind the stack
        {
            Print();
            status = INVALID;
            return;
        }
        status = LOADED; // try with another root
    }

    // No start root contained a solution. Then the problem has no solution
    status = INVALID;
    printf("Not possible!\n");
}

void Solver::Search()
{
#ifdef MAX_SECONDS
    void (*oldHandler)(int) = signal(SIGALRM, SignalHandler);
    loop = true;
    alarm(MAX_SECONDS);
#endif

    SearchWithAlarm();

#ifdef MAX_SECONDS
    alarm(0);
    signal(SIGALRM, oldHandler);
#endif
}

void Solver::RootTree(int root)
{
    assert(status == LOADED);

    for (int i = 0; i < n; ++i) pred[i] = -1;

    pred[root] = root;
    std::queue<int> q;
    q.push(root);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : graph[u]) {
            if (pred[v] == -1) {
                pred[v] = u;
                q.push(v);
            }
        }
    }

    this->root = root;
    status = ROOTED;
}

void Solver::Order()
{
    PredictPrio();
    assert(status == PREDICT);

    // Prioritize certain nodes for the toporder
    std::function<bool(int, int)> comp = [this](int a, int b) {
        return certainty[a] < certainty[b];
    };
    std::priority_queue<int, std::vector<int>, decltype(comp)> frontier(comp);
    frontier.push(root);

    while (!frontier.empty()) {
        int u = frontier.top();
        frontier.pop();

        for (int v : graph[u]) {
            if (pred[v] == u) {
                frontier.push(v);
            }
        }
        if (!frontier.empty()) {
            nextVertex[u] = frontier.top();
        }
        else {
            nextVertex[u] = -1;
        }
    }

    status = ORDERED;
}

void Solver::PredictPrio()
{
    assert(status == ROOTED);
    // Generate features for ml

    // Calculate depth for each node
    for (int i = 0; i < n; ++i) depth[i] = -1;
    depth[root] = 0;
    for (int u = 0; u < n; ++u) NodeDepth(u);

    // Calculate height for each node
    for (int i = 0; i < n; ++i) height[i] = -1;
    NodeHeight(root);

    // Calculate number of nodes in the subtree rooted in i, for each i
    for (int i = 0; i < n; ++i) nodeCount[i] = -1;
    NodeCount(root);

    float avgDepth = 0;
    int maxDepth = depth[0];
    for (int i = 0; i < n; ++i) {
        avgDepth += depth[i] / n;
        maxDepth = std::max(maxDepth, depth[i]);
    }

    float avgHeight = 0;
    int maxHeight = height[0];
    for (int i = 0; i < n; ++i) {
        avgHeight += height[i] / n;
        maxHeight = std::max(maxHeight, height[i]);
    }

#ifndef HAVE_PYTHON
    printf("using approximation\n");
    for (int i = 0; i < n; ++i) certainty[i] = (double)nodeCount[i] / n;
#else
    printf("using randomforest\n");
    // Generate the certainty array with a random forest regressor
    //
    // Dump features to input.csv, call getcertainty.py, read the result from
    // output.csv. An improvement would be to use pipes instead of files for IPC
    FILE* fp = fopen("input.csv", "w");
    assert(fp);
    fputs("n,subtree_cnt,depth,avg_depth,max_depth,height,avg_height,max_"
          "height\n",
          fp);
    for (int u = 0; u < n; ++u) {
        if (u == root) continue;
        fprintf(fp, "%d,%d,", n, nodeCount[u]);
        fprintf(fp, "%d,%f,%d,", depth[u], avgDepth, maxDepth);
        fprintf(fp, "%d,%f,%d\n", height[u], avgHeight, maxHeight);
    }
    assert(!ferror(fp));
    fclose(fp);

    fp = fopen("getcertainty.py", "r");
    assert(fp);
    int s = PyRun_SimpleFile(fp, "getcertainty.py");
    assert(s == 0);
    fclose(fp);

    fp = fopen("output.csv", "r");
    assert(fp);
    for (int i = 0; i < n; ++i) {
        if (i == root) {
            certainty[i] = 1.f;
        }
        else {
            fscanf(fp, "%f", &certainty[i]);
        }
    }
    assert(!ferror(fp));
    fclose(fp);
#endif

    status = PREDICT;
}

void Solver::BacktrackLoop(int u)
{
#ifdef MAX_SECONDS
    if (!loop) return;
#endif

    if (u == -1) {
        assert(CheckSolution());
        throw 0;
    }
    assert(BacktrackInvariant(u));

    if (depth[u] % 2) {
        for (int value = nextMax; value >= nextMin; value -= 2) {
            Backtrack(u, value);
            assert(BacktrackInvariant(u));
        }
    }
    else {
        for (int value = nextMin; value <= nextMax; value += 2) {
            Backtrack(u, value);
            assert(BacktrackInvariant(u));
        }
    }
}

void Solver::Backtrack(int u, int value)
{
    if (usedValues[value]) return;
    int diff = abs(values[pred[u]] - value);
    if (usedDiff[diff]) return;

    values[u] = value;
    usedValues[value] = true;
    usedDiff[diff] = true;

    // Update nextMin and nextMax. If u is the last vertex, there are no more
    // values, so skip instead.
    int oldMax = nextMax;
    if (nextVertex[u] != -1)
        while (usedValues[nextMax]) nextMax -= 2;
    int oldMin = nextMin;
    if (nextVertex[u] != -1)
        while (usedValues[nextMin]) nextMin += 2;

    BacktrackLoop(nextVertex[u]);

    nextMax = oldMax;
    nextMin = oldMin;

    values[u] = -1;
    usedValues[value] = false;
    usedDiff[diff] = false;
}
