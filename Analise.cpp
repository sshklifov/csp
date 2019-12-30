#include "Solver.h"

#include <cassert>
#include <random>

void Solver::NodeDepth(int u)
{
    if (depth[u] != -1) return;
    NodeDepth(pred[u]);
    depth[u] = depth[pred[u]] + 1;
}

void Solver::NodeHeight(int u)
{
    height[u] = 0;
    for (int v : graph[u])
    {
        if (pred[v] == u)
        {
            NodeHeight(v);
            height[u] = std::max(height[u], height[v] + 1);
        }
    }
}

void Solver::NodeCount(int u)
{
    if (nodeCount[u] != -1) return;

    nodeCount[u] = 1;
    for (int v : graph[u])
    {
        if (u == pred[v])
        {
            NodeCount(v);
            nodeCount[u] += nodeCount[v];
        }
    }
}

static int PathCompression(std::vector<int>& p, int i)
{
    if (p[i] == -1) return i;
    return p[i] = PathCompression(p, p[i]);
}

void Solver::Random(int n)
{
    assert(n <= MAX_VERTICES);
    this->n = n;
    for (int i = 0; i < n; ++i) graph[i].clear();

    static std::default_random_engine eng (55);
    std::uniform_int_distribution<int> distr(0, n-1);

    std::vector<int> p(n, -1);
    std::vector<int> rank(n, 1);

    int edges = n-1;
    int v1 = distr(eng);
    while (edges > 0)
    {
        int v2 = distr(eng);

        int parent1 = PathCompression(p, v1);
        int parent2 = PathCompression(p, v2);
        if (parent1 != parent2)
        {
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

        v1 = v2;
    }

    status = LOADED;
    assert(CheckIsTree());
}

void Solver::Print()
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

#if 0
std::vector<int> valueDistribution[MAX_VERTICES];
void FlushDistribution()
{
    FILE* fp = fopen("distr.txt", "w");
    assert(fp);

    int maxValue = 2*n - 1;
    for (int value = 1; value <= maxValue; value+=2)
    {
        fprintf(fp, "%d ", value);
    }
    fputs("\n", fp);
    for (int u = 0; u < n; ++u)
    {
        fprintf(fp, "%d ", u);
        for (int value = 1; value <= maxValue; value+=2)
        {
            fprintf(fp, "%d ", valueDistribution[u][value]);
        }
        fputs("\n", fp);
    }
    fclose(fp);
}

void FlushClassifier(int root)
{
    std::vector<int> depth(n, -1);
    depth[root] = 0;
    for (int u = 0; u < n; ++u)
    {
        NodeDepth(depth.data(), u);
    }

    std::vector<int> height(n, -1);
    NodeHeight(height, root);

    std::vector<int> count(n, -1);
    NodeCount(count.data(), root);

    for (int u = 0; u < n; ++u)
    {
        if (u == root) continue;
        int highestVal = 2*n - 1;

        int sum = 0;
        for (int i = 1; i <= highestVal; i += 2)
        {
            sum += valueDistribution[u][i];
        }

        double loExpect = 0.f;
        for (int i = 3; i <= highestVal; i += 2)
        {
            int steps = i/2;
            loExpect += (double)valueDistribution[u][i]*steps / sum;
        }

        double hiExpect = 0.f;
        for (int i = highestVal; i >= 3; i -= 2)
        {
            int steps = (highestVal-i)/2 + 1;
            hiExpect += (double)valueDistribution[u][i]*steps / sum;
        }

        double rndExpect = (n-1) / 2.0;

        int udepth = depth[u];
        int uheight = height[u];
        int ucount = count[u];
        printf("%d,%d,%d,%d,%lf,%lf,%lf\n", n, ucount, udepth, uheight,
                loExpect, hiExpect, rndExpect);
    }
}
#endif
