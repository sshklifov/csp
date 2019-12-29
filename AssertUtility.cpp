#include "Globals.h"

#include <cstdlib>
#include <cassert>

bool CheckSolution()
{
    std::vector<bool> used (USED_CAPACITY, 0);
    for (int i = 0; i < n; ++i)
    {
        assert(!used[values[i]]);
        used[values[i]] = true;
    }

    used.assign(used.size(), 0);
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            if (u > v) continue;

            int diff = abs(values[u] - values[v]);
            assert(!used[diff]);
            used[diff] = true;
        }
    }
    
    return true;
}

static void DfsVisit(int u, int parent, std::vector<bool>& visited)
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
    std::vector<bool> visited (n, false);
    DfsVisit(0, -1, visited);
    for (int i = 0; i < n; ++i)
    {
        assert(visited[i]);
    }

    return true;
}

static int CountVerticesBefore(int u)
{
    std::vector<int> prevVertex(n, -1);
    int lastVertex = -1;
    for (int u = 0; u < n; ++u)
    {
        int afterU = nextVertex[u];
        if (afterU == -1)
        {
            assert(lastVertex == -1);
            lastVertex = u;
            continue;
        }
        assert(afterU >= 0 && afterU < n);
        prevVertex[afterU] = u;
    }

    std::vector<bool> seen(n, 0);
    seen[lastVertex] = 1;
    int firstVertex = lastVertex;
    while (prevVertex[firstVertex] != -1)
    {
        assert(!seen[prevVertex[firstVertex]]);
        seen[prevVertex[firstVertex]] = 1;
        firstVertex = prevVertex[firstVertex];
    }
    for (int i = 0; i < n; ++i) assert(seen[i]);

    int cnt = 0;
    while (firstVertex != u)
    {
        firstVertex = nextVertex[firstVertex];
        ++cnt;
    }

    return cnt;
}

bool BacktrackInvariant(int u) // TODO mildy put
{
    int vertices = CountVerticesBefore(u);

    int numValues = 0;
    for (int i = 0; i < n; ++i)
    {
        if (values[i] >= 0) ++numValues;
    }
    assert(numValues == vertices);

    int numUsedValues = 0;
    for (int i = 0; i < USED_CAPACITY; ++i)
    {
        if (usedValue[i]) ++numUsedValues;
    }
    assert(numUsedValues == vertices);

    int numUsedDiffs = 0;
    for (int i = 0; i < USED_CAPACITY; ++i)
    {
        if (usedDiff[i]) ++numUsedDiffs;
    }
    assert(numUsedDiffs == vertices - 1);

    for (int i = 0; i < n; ++i)
    {
        if (values[i] >= 0)
        {
            assert(usedValue[values[i]]);
        }
    }

    for (int i = 0; i < n; ++i)
    {
        if (values[i] >= 0)
        {
            int j = i;
            do
            {
                j = pred[j];
                assert(values[j] >= 0);
            }
            while (pred[j] != j);
        }
    }

    std::vector<int> used (USED_CAPACITY, 0);
    for (int i = 0; i < n; ++i)
    {
        if (values[i] >= 0)
        {
            assert(!used[values[i]]);
            used[values[i]] = true;
        }
    }
    used.assign(USED_CAPACITY, 0);
    for (int u = 0; u < n; ++u)
    {
        if (values[u] < 0) continue;
        for (int v : graph[u])
        {
            if (values[v] < 0) continue;
            if (v > u) continue;

            int diff = abs(values[u] - values[v]);
            assert(usedDiff[diff]);

            assert(!used[diff]);
            used[diff] = true;
        }
    }

    return true;
}
