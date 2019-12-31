#include "Solver.h"

#include <cstdio>
#include <cassert>
#include <queue>
#include <functional>
#include <algorithm>

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

bool Solver::Search()
{
    assert(status == LOADED);

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
        assert(status == READY);
        try
        {
            /* nextMin = 3; */
            /* nextMax = 2*n - 1; */

            // TODO gore
            values[root] = 1;
            usedValues[1] = true;
            BacktrackLoop(nextVertex[root]);
        }
        catch(int)
        {
            return true;
        }
        status = LOADED;
    }
    
    return false;
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

    Order();
    assert(status == ORDERED);

    for (int& x : values) x = -1;
    for (bool& x : usedValues) x = false;
    for (bool& x : usedDiff) x = false;

    status = READY;
}

void Solver::Order()
{
    PredictPrio();
    assert(status == PREDICT);

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

/* #if !defined(HAVE_PYTHON) || MAX_VERTICES < 50 */
#if 0
    for (int i = 0; i < n; ++i) stability[i] = 1.f;
#else
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
    assert(BacktrackInvariant(u));
    if (u == -1)
    {
        assert(CheckSolution());
        throw 0;
    }

    /* if (stability[u] < 0.1f) */
    if (false && stability[u] < 0.1f)
    {
        /* int lo = n + !(n%2); */
        /* int hi = lo + 2; */
        /* while (lo >= 3 && hi <= 2*n - 1) */
        /* { */
        /*     Backtrack(u, lo); */
        /*     lo -= 2; */
        /*     Backtrack(u, hi); */
        /*     hi += 2; */
        /* } */
        /* while (lo >= 3) */
        /* { */
        /*     Backtrack(u, lo); */
        /*     lo -= 2; */
        /* } */
        /* while (hi <= 2*n - 1) */
        /* { */
        /*     Backtrack(u, hi); */
        /*     hi += 2; */
        /* } */
    }
    else if (depth[u] % 2)
    {
        /* int oldMax = nextMax; */
        /* for (int value = nextMax + 2; used[value];) */
        /* Backtrack */
        for (int value = 2*n - 1; value >= 3; value -= 2)
        {
            Backtrack(u, value);
        }
    }
    else
    {
        for (int value = 3; value <= 2*n - 1; value += 2)
        {
            Backtrack(u, value);
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

    BacktrackLoop(nextVertex[u]);

    values[u] = -1;
    usedValues[value] = false;
    usedDiff[diff] = false;

    assert(BacktrackInvariant(u));
}
