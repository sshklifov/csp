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
#include <algorithm>
#include <climits>
#include <csignal>

#include "Globals.h"
#include "AssertUtility.h"

void ReadTree()
{
    FILE* fp = fopen("input.txt", "r");
    assert(fp);

    fscanf(fp, "%d", &n);
    assert(n < MAX_VERTICES);
    for (int i = 0; i < n; ++i) graph[i].clear();
    
    int v1, v2;
    while (fscanf(fp, "%d %d", &v1, &v2) == 2)
    {
        graph[v1].push_back(v2);
        graph[v2].push_back(v1);
    }

    assert(!ferror(fp));
    assert(CheckIsTree());
}

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

void NodeDepth(std::vector<int>& depth, int u)
{
    if (depth[u] != -1) return;
    NodeDepth(depth, pred[u]);
    depth[u] = depth[pred[u]] + 1;
}

void NodeHeight(std::vector<int>& height, int u)
{
    height[u] = 0;
    for (int v : graph[u])
    {
        if (pred[v] == u)
        {
            NodeHeight(height, v);
            height[u] = std::max(height[u], height[v] + 1);
        }
    }
}

void NodeCount(std::vector<int>& count, int u)
{
    if (count[u] != -1) return;

    count[u] = 1;
    for (int v : graph[u])
    {
        if (u == pred[v])
        {
            NodeCount(count, v);
            count[u] += count[v];
        }
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

    DfsOrder(root);
}

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
        NodeDepth(depth, u);
    }

    std::vector<int> height(n, -1);
    NodeHeight(height, root);

    std::vector<int> count(n, -1);
    NodeCount(count, root);

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

void Backtrack2(int u)
{
    assert(BacktrackInvariant(u));

    if (u == -1)
    {
        assert(CheckSolution());
        for (int i = 0; i < n; ++i)
        {
            ++valueDistribution[i][values[i]];
        }
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

void MainLoop(int)
{
    static std::default_random_engine eng(515);
    std::uniform_int_distribution<int> distr(9, 15);
    RandomTree(distr(eng));
    /* RandomTree(13); */

    std::vector<int> roots(n);
    for (int i = 0; i < n; ++i) roots[i] = i;
    std::sort(roots.begin(), roots.end(),
        [](int a, int b)
        {
            return graph[a].size() > graph[b].size();
        });

    int root = roots[0];
    PrepareBacktrack2(root);
    try
    {
        Backtrack2(nextVertex[root]);
    }
    catch (int)
    {}

    /* PrintTree(); */
    /* FlushDistribution(); // TODO */
    FlushClassifier(root);
    for (int i = 0; i < n; ++i)
    {
        valueDistribution[i].assign(USED_CAPACITY, 0);
    }
}

sig_atomic_t loop = 1;
void SignalHandler(int)
{
    loop = 0;
}

int main()
{
    signal(SIGINT, SignalHandler);
    printf("n,subtree_cnt,depth,height,loexpect,hiexpect,rnd\n");

    for (int i = 0; i < MAX_VERTICES; ++i)
    {
        valueDistribution[i].resize(USED_CAPACITY, 0);
    }

    do
    {
        MainLoop(-1);
    }
    while (loop);
    return 0;
}
