#include "Solver.h"

#include <cassert>
#include <random>

// helper function to calculate depth for each node
void Solver::NodeDepth(int u)
{
    if (depth[u] != -1) return;
    NodeDepth(pred[u]);
    depth[u] = depth[pred[u]] + 1;
}

// helper function to calculate height for each node
void Solver::NodeHeight(int u)
{
    height[u] = 0;
    for (int v : graph[u]) {
        if (pred[v] == u) {
            NodeHeight(v);
            height[u] = std::max(height[u], height[v] + 1);
        }
    }
}

// helper function to calculate number of nodes rooted at u for each node
void Solver::NodeCount(int u)
{
    if (nodeCount[u] != -1) return;

    nodeCount[u] = 1;
    for (int v : graph[u]) {
        if (u == pred[v]) {
            NodeCount(v);
            nodeCount[u] += nodeCount[v];
        }
    }
}

// for disjoin set data structure (similar to kruskal)
static int PathCompression(std::vector<int>& p, int i)
{
    if (p[i] == -1) return i;
    return p[i] = PathCompression(p, p[i]);
}

// generate uniform random tree (with a random walk)
// Започваме с random root. Всеки път избираме нов random node.
// Искаме да сложим edge между него и последния node. Ако образуваме
// цикъл, не правим нищо и random node-a става последен random node.
// Ако не образува цикъл, добавяме edge-a и random node-a става последения
// random node.
void Solver::Random(int n)
{
    assert(n <= MAX_VERTICES && n > 9);
    this->n = n;
    for (int i = 0; i < n; ++i) graph[i].clear();

    static std::default_random_engine eng(SEED);
    std::uniform_int_distribution<int> distr(0, n - 1);

    std::vector<int> p(n, -1);
    std::vector<int> rank(n, 1);

    int edges = n - 1;
    int v1 = distr(eng);
    while (edges > 0) {
        int v2 = distr(eng);

        int parent1 = PathCompression(p, v1);
        int parent2 = PathCompression(p, v2);
        if (parent1 != parent2) {
            graph[v1].push_back(v2);
            graph[v2].push_back(v1);
            --edges;

            if (rank[parent1] > rank[parent2]) {
                p[parent2] = parent1;
            }
            else {
                p[parent1] = parent2;
                if (rank[parent1] == rank[parent2]) {
                    ++rank[parent2];
                }
            }
        }

        v1 = v2;
    }

    status = LOADED;
    assert(CheckIsTree());
}

void Solver::Print()
{
    printf("%d\n", n);

    for (int u = 0; u < n; ++u) {
        for (int v : graph[u]) {
            if (u > v) continue;
            printf("%d %d\n", u, v);
        }
    }
}
