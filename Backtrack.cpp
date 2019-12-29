#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <random>
#include <memory>
#include <cstring>
#include <stack>
#include <queue>
#include <functional>

#include "Globals.h"
#include "AssertUtility.h"

int PathCompression(std::vector<int>& p, int i)
{
    if (p[i] == -1) return i;
    return p[i] = PathCompression(p, p[i]);
}

void RandomTree(int vertices)
{
    assert(vertices <= MAX_VERTICES);
    n = vertices;
    for (int i = 0; i < n; ++i) graph[i].clear();

    static std::default_random_engine eng (SEED);
    std::uniform_int_distribution<int> distr(0, n-1);

    std::vector<int> p(n, -1);
    std::vector<int> rank(n, 1);

    int edges = n-1;
    int v1 = distr(eng);
    while (edges > 0)
    {
        int v2 = distr(eng);
        /* printf("%d ", v2); */

        int parent1 = PathCompression(p, v1);
        int parent2 = PathCompression(p, v2);
        if (parent1 != parent2)
        {
            /* printf("%d %d | ", v1, v2); */
            graph[v1].push_back(v2);
            graph[v2].push_back(v1);
            --edges;

            if (rank[parent1] > rank[parent2])
            {
                p[parent2] = parent1;
            }
            else
            {
                p[parent1] = parent2;
                if (rank[parent1] == rank[parent2])
                {
                    ++rank[parent2];
                }
            }
        }
        /* else printf("0 "); */

        v1 = v2;
    }

    /* fflush(stdout); */
    assert(CheckIsTree());
}

void PrintTree()
{
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            if (u > v) continue;
            printf("%d %d\n", u, v);
        }
    }
}

void PrintTreeValues()
{
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            if (u > v || values[u] == -1 || values[v] == -1) continue;
            printf("%d %d\n", values[u], values[v]);
        }
    }
}

void BfsOrder(int root)
{
    for (int i = 0; i < n; ++i)
        nextVertex[i] = -1;

    std::queue<int> q;
    q.push(root);

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v : graph[u])
        {
            if (nextVertex[v] == -1)
            {
                q.push(v);
            }
        }
        if (!q.empty()) nextVertex[u] = q.front();
        else nextVertex[u] = -1;
    }
}

void BfsPrioOrder(int root)
{
    for (int i = 0; i < n; ++i)
        nextVertex[i] = -1;

    std::vector<int> depth(n, 0);
    std::function<bool(int,int)> comp =
        [&depth](int a, int b)
        {
            return (depth[a] > depth[b]) ||
                (depth[a]==depth[b] && graph[a].size()<graph[b].size());
        };

    std::priority_queue<int,std::vector<int>,decltype(comp)> frontier (comp);
    frontier.push(root);
    while (!frontier.empty())
    {
        int u = frontier.top();
        frontier.pop();

        for (int v : graph[u])
        {
            if (nextVertex[v] == -1)
            {
                depth[v] = depth[u] + 1;
                frontier.push(v);
            }
        }
        if (!frontier.empty()) nextVertex[u] = frontier.top();
        else nextVertex[u] = -1;
    }
}

void DfsOrder(int root)
{
    for (int i = 0; i < n; ++i)
        nextVertex[i] = -1;

    std::stack<int> q;
    q.push(root);

    while (!q.empty())
    {
        int u = q.top();
        q.pop();

        for (int v : graph[u])
        {
            if (nextVertex[v] == -1)
            {
                q.push(v);
            }
        }
        if (!q.empty()) nextVertex[u] = q.top();
        else nextVertex[u] = -1;
    }
}

void PrepareBacktrack2(int root)
{
    for (int& x : values) x = -1;
    for (int& x : usedValue) x = false;
    for (int& x : usedDiff) x = false;

    values[root] = 1;
    usedValue[1] = true;

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

    /* BfsOrder(root); */
    /* BfsPrioOrder(root); */
    DfsOrder(root);
}

void Backtrack2(int u)
{
    assert(BacktrackInvariant(u));

    if (u == -1)
    {
        assert(CheckSolution());
        /* for (int i = 0; i < n; ++i) printf("%d ", values[i]); */
        /* printf("\n"); */
        /* throw u; */
        return;
    }

    for (int value = 1; value <= 2*n - 1; value += 2)
    {
        if (usedValue[value]) continue;
        int diff = abs(values[pred[u]] - value);
        if (usedDiff[diff]) continue;

        values[u] = value;
        usedValue[value] = true;
        usedDiff[diff] = true;

        Backtrack2(nextVertex[u]);

        values[u] = -1;
        usedValue[value] = false;
        usedDiff[diff] = false;

        assert(BacktrackInvariant(u));
    }
}

void MainLoop()
{
    RandomTree(12);

    /* int root = 0; */
    /* for (int i = 1; i < n; ++i) */
    /* { */
    /*     if (graph[i].size() > graph[root].size()) root = i; */
    /* } */
    for (int root = 0; root < n; ++root)
    {
        PrepareBacktrack2(root);

        try
        {
            Backtrack2(nextVertex[root]);
        }
        catch (int)
        {}
    }
}

int main()
{
    int loop = 50;
    while (loop --> 0)
    {
        MainLoop();
    }
    return 0;
}
