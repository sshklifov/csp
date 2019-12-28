#include "Globals.h"

#include <vector>
#include <cstdlib>
#include <cassert>

bool CheckSolution()
{
    std::vector<bool> used (USED_CAPACITY, 0);
    for (int i = 0; i < n; ++i)
    {
        assert(!used[vertexValues[i]]);
        used[vertexValues[i]] = true;
    }

    used.assign(used.size(), 0);
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            if (u > v) continue;

            int diff = abs(vertexValues[u] - vertexValues[v]);
            assert(!used[diff]);
            used[diff] = true;
        }
    }
    
    return true;
}

bool BacktrackInvariant(int diff)
{
    int firstDiff = 2*n - 2;
    int vertices = (firstDiff-diff)/2 + 1;

    int numVertexValues = 0;
    for (int i = 0; i < n; ++i)
    {
        if (vertexValues[i] >= 0) ++numVertexValues;
    }
    assert(numVertexValues == vertices);

    int numUsedValues = 0;
    for (int i = 0; i < USED_CAPACITY; ++i)
    {
        if (usedValues[i]) ++numUsedValues;
    }
    assert(numUsedValues == vertices);

    for (int i = 0; i < n; ++i)
    {
        if (vertexValues[i] >= 0)
        {
            assert(usedValues[vertexValues[i]]);
        }
    }

    for (int i = 0; i < n; ++i)
    {
        if (vertexValues[i] >= 0)
        {
            int j = i;
            do
            {
                j = pred[j];
                assert(vertexValues[j] >= 0);
            }
            while (pred[j] != j);
        }
    }

    std::vector<int> used (USED_CAPACITY, 0);
    for (int i = 0; i < n; ++i)
    {
        if (vertexValues[i] >= 0)
        {
            assert(!used[vertexValues[i]]);
            used[vertexValues[i]] = true;
        }
    }
    used.assign(USED_CAPACITY, 0);
    for (int u = 0; u < n; ++u)
    {
        if (vertexValues[u] < 0) continue;
        for (int v : graph[u])
        {
            if (vertexValues[v] < 0) continue;
            if (v > u) continue;

            int diff = abs(vertexValues[u] - vertexValues[v]);
            assert(!used[diff]);
            used[diff] = true;
        }
    }

    return true;
}

void DfsVisit(int u, int parent, std::vector<bool>& visited)
{
    visited[u] = true;

    bool mirrorEdge = false;
    for (int v : graph[u])
    {
        if (v == parent)
        {
            mirrorEdge = true;
            continue;
        }

        assert(!visited[v]);
        DfsVisit(v, u, visited);
    }
    if (parent >= 0 && parent < n) assert(mirrorEdge);
}

bool CheckIsTree()
{
    std::vector<bool> visited(n,false);
    DfsVisit(0, -1, visited);
    for (int i = 0; i < n; ++i)
    {
        assert(visited[i]);
    }

    return true;
}
