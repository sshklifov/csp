#include "Solver.h"

#include <cstdio>
#include <cassert>
#include <queue>
#include <functional>

Solver::Solver(int n)
{
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
    assert(n < MAX_VERTICES);

    for (int i = 0; i < MAX_VERTICES; ++i) graph[i].clear();

    int v1, v2;
    while (fscanf(fp, "%d %d", &v1, &v2) == 2)
    {
        graph[v1].push_back(v2);
        graph[v2].push_back(v1);
    }

    assert(!ferror(fp));
    status = LOADED;
    assert(CheckIsTree());
}

void Solver::RootTree(int root)
{
    for (int i = 0; i < n; ++i) pred[i] = -1;

    pred[root] = root;
    std::queue<int> q;
    q.push(root);

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : graph[u])
        {
            if (pred[v] == -1)
            {
                pred[v] = u;
                q.push(v);
            }
        }
    }

    this->root = root;
    status = ROOTED;

    Order();
    assert(status == ORDERED);

    for (int& x : values) x = -1;
    for (bool& x : usedValues) x = false;
    for (bool& x : usedDiff) x = false;

    values[root] = 1;
    usedValues[1] = true;

    status = READY;
}

void Solver::PredictPrio(int* prio)
{
    assert(status == ROOTED);

    for (int i = 0; i < n; ++i) depth[i] = -1;
    depth[root] = 0;
    for (int u = 0; u < n; ++u)
    {
        NodeDepth(u);
    }

    for (int i = 0; i < n; ++i) height[i] = -1;
    NodeHeight(root);

    for (int i = 0; i < n; ++i) nodeCount[i] = -1;
    NodeCount(root);

    status = PREDICT;

    // TODO ML
    for (int i = 0; i < n; ++i) prio[i] = nodeCount[i];
}

void Solver::Order()
{
    int prio[MAX_VERTICES];
    PredictPrio(prio);

    std::function<bool(int,int)> comp =
        [prio](int a, int b)
        {
            return prio[a] < prio[b];
        };
    std::priority_queue<int,std::vector<int>,decltype(comp)> frontier(comp);
    frontier.push(root);

    while (!frontier.empty())
    {
        int u = frontier.top();
        frontier.pop();

        for (int v : graph[u])
        {
            if (pred[v] == u)
            {
                frontier.push(v);
            }
        }
        if (!frontier.empty()) nextVertex[u] = frontier.top();
        else nextVertex[u] = -1;
    }

    status = ORDERED;
}

void Solver::Search()
{
    assert(status == READY);
    try
    {
        Backtrack(nextVertex[root]);
        printf("No solution\n");
    }
    catch (int)
    {
        for (int u = 0; u < n; ++u)
        {
            for (int v : graph[u])
            {
                if (u > v) continue;
                printf("%d %d\n", values[u], values[v]);
            }
        }
    }
}

void Solver::Backtrack(int u)
{
    assert(BacktrackInvariant(u));

    if (u == -1)
    {
        assert(CheckSolution());
        throw u;
    }

    for (int value = 3; value <= 2*n - 1; value += 2)
    {
        if (usedValues[value]) continue;
        int diff = abs(values[pred[u]] - value);
        if (usedDiff[diff]) continue;

        values[u] = value;
        usedValues[value] = true;
        usedDiff[diff] = true;

        Backtrack(nextVertex[u]);

        values[u] = -1;
        usedValues[value] = false;
        usedDiff[diff] = false;

        assert(BacktrackInvariant(u));
    }
}
