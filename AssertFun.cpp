#include "Solver.h"

#include <vector>
#include <cassert>
#include <cstdlib>
#include <functional>

// Defines some checks during executation. They do not guarantee correct
// operation. Please compile with -DNDEBUG, because they slow down the
// program significantly. Nothing to see here.

bool Solver::CheckSolution()
{
    std::vector<bool> used(USED_CAPACITY, 0);
    for (int i = 0; i < n; ++i) {
        assert(!used[values[i]]);
        used[values[i]] = true;
    }

    used.assign(used.size(), 0);
    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            if (u > v) continue;

            int diff = abs(values[u] - values[v]);
            assert(!used[diff]);
            used[diff] = true;
        }
    }

    return true;
}

bool Solver::CheckIsTree()
{
    std::vector<bool> visited(n, false);
    std::function<void(int, int)> DfsVisit = [&](int u, int parent) {
        visited[u] = true;

        int mirrorEdge = 0;
        for (int v : graph[u]) {
            if (v == parent) {
                ++mirrorEdge;
                continue;
            }

            assert(!visited[v]);
            DfsVisit(v, u);
        }
        if (parent != -1) assert(mirrorEdge == 1);
    };

    DfsVisit(0, -1);
    for (int i = 0; i < n; ++i) {
        assert(visited[i]);
    }

    return true;
}

bool Solver::BacktrackInvariant(int u)
{
    int vertices = 0;
    for (int i = root; i != u; i = nextVertex[i]) {
        ++vertices;
    }

    int numValues = 0;
    for (int i = 0; i < n; ++i) {
        if (values[i] >= 0) ++numValues;
    }
    assert(numValues == vertices);

    int numUsedValues = 0;
    for (int i = 0; i < USED_CAPACITY; ++i) {
        if (usedValues[i]) ++numUsedValues;
    }
    assert(numUsedValues == vertices);

    int numUsedDiffs = 0;
    for (int i = 0; i < USED_CAPACITY; ++i) {
        if (usedDiff[i]) ++numUsedDiffs;
    }
    assert(numUsedDiffs == vertices - 1);

    for (int i = 0; i < n; ++i) {
        if (values[i] >= 0) {
            assert(usedValues[values[i]]);
        }
    }

    for (int i = 0; i < n; ++i) {
        if (values[i] >= 0) {
            int j = i;
            do {
                j = pred[j];
                assert(values[j] >= 0);
            } while (pred[j] != j);
        }
    }

    std::vector<int> used(USED_CAPACITY, 0);
    for (int i = 0; i < n; ++i) {
        if (values[i] >= 0) {
            assert(!used[values[i]]);
            used[values[i]] = true;
        }
    }
    used.assign(USED_CAPACITY, 0);
    for (int u = 0; u < n; ++u) {
        if (values[u] < 0) continue;
        for (int v : graph[u]) {
            if (values[v] < 0) continue;
            if (v > u) continue;

            int diff = abs(values[u] - values[v]);
            assert(usedDiff[diff]);

            assert(!used[diff]);
            used[diff] = true;
        }
    }

    if (nextVertex[u] != -1) {
        int mmax = 2 * n - 1;
        while (usedValues[mmax]) mmax -= 2;
        assert(mmax == nextMax);

        int mmin = 3;
        while (usedValues[mmin]) mmin += 2;
        assert(mmin == nextMin);
    }

    return true;
}
