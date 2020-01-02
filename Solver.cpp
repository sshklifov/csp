#include "Solver.h"

#include <cstdio>
#include <cassert>
#include <queue>
#include <functional>
#include <algorithm>

Solver::Solver() : status(INVALID)
{}

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
    assert(n <= MAX_VERTICES && n > 9);

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

    // consider every node as the root. consider nodes
    // with the higher degrees first.
    std::vector<int> roots(n);
    for (int i = 0; i < n; ++i) roots[i] = i;
    std::sort(roots.begin(), roots.end(),
        [this](int a, int b)
        {
            return graph[a].size() > graph[b].size();
        });

    for (int root : roots)
    {
        RootTree(root);
        assert(status == ORDERED);

        // backtrack state variables
        for (int& x : values) x = -1;
        for (bool& x : usedValues) x = false;
        for (bool& x : usedDiff) x = false;

        // stores the smallest available value for a node
        // used to speed up backtracking
        nextMin = 3;
        // stores the biggest available value for a node.
        // used to speed up backtracking
        nextMax = 2*n - 1;

        values[root] = 1;
        usedValues[1] = true;

        try
        {
#ifdef MAX_SECONDS
            alarm(MAX_SECONDS);
            loop = true;
            BacktrackLoop(nextVertex[root]);
            printf("Timed out!\n");
            alarm(0);
            status = INVALID;
            return;
#else
            BacktrackLoop(nextVertex[root]);
#endif
        }
        catch(int) // if a solution is found, throw an int to unwind the stack
        {
            for (int i = 0; i < n; ++i) // print solution
            {
                for (int j : graph[i])
                {
                    if (i > j) continue;
                    printf("%d %d\n", values[i], values[j]);
                }
            }
            status = INVALID;
            return; // and finish
        }
        status = LOADED; // try with another root
    }
    
    // no start root contained a solution. then the problem has no solution
    status = INVALID;
    printf("Not possible!\n");
}

void Solver::Search()
{
#ifdef MAX_SECONDS
    void(*oldHandler)(int) = signal(SIGALRM, SignalHandler);
#endif

    SearchWithAlarm();

#ifdef MAX_SECONDS
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

    // after rooting, choose node ordering
    Order();
    assert(status == ORDERED);
}

void Solver::Order()
{
    PredictPrio();
    assert(status == PREDICT);

    // prioritize stable nodes for the toporder. in our case this is just
    // generated a random toporder.
    std::function<bool(int,int)> comp =
        [this](int a, int b)
        {
            return stability[a] < stability[b];
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

void Solver::PredictPrio()
{
    assert(status == ROOTED);

    // Calculate depth for each node
    // the depth is determined from the current root
    for (int i = 0; i < n; ++i) depth[i] = -1;
    depth[root] = 0;
    for (int u = 0; u < n; ++u)
    {
        NodeDepth(u);
    }

    // Calculate height for each node
    // the height is determined from the current root
    for (int i = 0; i < n; ++i) height[i] = -1;
    NodeHeight(root);

    // Calculate number of nodes in the subtree rooted in i, for each i
    // as the above, this is dependent on the current root
    for (int i = 0; i < n; ++i) nodeCount[i] = -1;
    NodeCount(root);

    float avgDepth = 0;
    int maxDepth = depth[0];
    for (int i = 0; i < n; ++i)
    {
        avgDepth += depth[i];
        maxDepth = std::max(maxDepth, depth[i]);
    }
    avgDepth /= n;

    float avgHeight = 0;
    int maxHeight = height[0];
    for (int i = 0; i < n; ++i)
    {
        avgHeight += height[i];
        maxHeight = std::max(maxHeight, height[i]);
    }
    avgHeight /= n;

#ifndef HAVE_PYTHON
    printf("using approximation\n");
    for (int i = 0; i < n; ++i) stability[i] = (double)nodeCount[i] / n;
#else
    printf("using randomforest\n");
    // generate the stability array with a random forest regressor
    FILE* fp = fopen("input.csv", "w");
    assert(fp);
    fputs("n,subtree_cnt,depth,avg_depth,max_depth,height,avg_height,max_height\n", fp);
    for (int u = 0; u < n; ++u)
    {
        if (u == root) continue;
        fprintf(fp, "%d,%d,", n, nodeCount[u]);
        fprintf(fp, "%d,%f,%d,", depth[u], avgDepth, maxDepth);
        fprintf(fp, "%d,%f,%d\n", height[u], avgHeight, maxHeight);
    }
    assert(!ferror(fp));
    fclose(fp);

    int s = system("python getstability.py");
    assert(s == 0);

    fp = fopen("output.csv", "r");
    assert(fp);
    for (int i = 0; i < n; ++i)
    {
        if (i == root) stability[i] = 1.f;
        else fscanf(fp, "%f", &stability[i]);
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

    if (u == -1)
    {
        assert(CheckSolution());
        throw 0;
    }
    assert(BacktrackInvariant(u));

    if (depth[u] % 2)
    {
        for (int value = nextMax; value >= nextMin; value -= 2)
        {
            Backtrack(u, value);
            assert(BacktrackInvariant(u));
        }
    }
    else
    {
        for (int value = nextMin; value <= nextMax; value += 2)
        {
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

    // add a check nextVertex[u] != -1 otherwise infinite while loop (all values are assigned)
    int oldMax = nextMax;
    if (nextVertex[u] != -1) while (usedValues[nextMax]) nextMax -= 2;
    int oldMin = nextMin;
    if (nextVertex[u] != -1) while (usedValues[nextMin]) nextMin += 2;

    BacktrackLoop(nextVertex[u]);

    nextMax = oldMax;
    nextMin = oldMin;

    values[u] = -1;
    usedValues[value] = false;
    usedDiff[diff] = false;

    assert(BacktrackInvariant(u));
}
