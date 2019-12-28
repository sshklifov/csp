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
#include <csignal>
#include <forward_list>
#include <list>

#define MAX_VERTICES 50
#define SEED 35

/* int nextVertex[MAX_VERTICES]; */
int n;
std::vector<int> graph[MAX_VERTICES];
/* std::vector<int> adj[MAX_VERTICES]; */

int PathCompression(std::vector<int>& pred, int i)
{
    if (pred[i] == -1) return i;
    return pred[i] = PathCompression(pred, pred[i]);
}

void RandomTree(int n)
{
    assert(n <= MAX_VERTICES);

    static std::default_random_engine eng (SEED);
    std::uniform_int_distribution<int> uni(0, n-1);

    ::n = n;
    for (int i = 0; i < MAX_VERTICES; ++i) graph[i].clear();

    std::vector<int> pred(n, -1);
    std::vector<int> rank(n, 1);

    int edges = n-1;
    int v1 = uni(eng);
    while (edges > 0)
    {
        int v2 = uni(eng);

        int parent1 = PathCompression(pred, v1);
        int parent2 = PathCompression(pred, v2);
        if (parent1 != parent2)
        {
            /* if (v2 < v1) std::swap(v1, v2); */
            graph[v1].push_back(v2);
            graph[v2].push_back(v1);
            --edges;

            if (rank[parent1] > rank[parent2])
            {
                pred[parent2] = parent1;
            }
            else
            {
                pred[parent1] = parent2;
                if (rank[parent1] == rank[parent2])
                {
                    ++rank[parent2];
                }
            }
        }

        v1 = v2;
    }
}

void PrintTree()
{
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            printf("%d %d\n", u, v);
        }
    }
}

/* std::list<int> frontier; */
int vertexValues[MAX_VERTICES];
bool usedValues[2*MAX_VERTICES];
int pred[MAX_VERTICES];

void PrintTreeValues()
{
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            printf("%d %d\n", vertexValues[u], vertexValues[v]);
        }
    }
}

void PrepareBacktrack(int root)
{
    for (int& x : vertexValues) x = -1;
    for (bool& x : usedValues) x = 0;
    for (int& x : pred) x = -1;

    /* frontier.clear(); */
    pred[root] = root;
    for (int v : graph[root])
    {
        /* frontier.push_front(v); */
        pred[v] = root;
    }
    vertexValues[root] = 2*n - 1;
    usedValues[vertexValues[root]] = 1;
}

void Backtrack(int diff)
{
    if (diff == 0)
    {
        /* PrintTreeValues(); */
        throw diff;
    }

    for (int u = 0; u < n; ++u) // ORDER?
    {
        if (!(pred[u] >= 0 && vertexValues[u]==-1)) continue;

        int values[] = {vertexValues[pred[u]] + diff,
            vertexValues[pred[u]] - diff};
        for (int value : values)
        {
            if (value >= 1 && value <= 2*n-1 && !usedValues[value])
            {
                for (int v : graph[u])
                {
                    if (v == pred[u]) continue;
                    pred[v] = u;
                }
                vertexValues[u] = value;
                usedValues[value] = true;

                Backtrack(diff - 2);
                usedValues[value] = false;
                vertexValues[u] = -1; // TODO spesti tazi smetka
                for (int v : graph[u])
                {
                    if (v == pred[u]) continue;
                    pred[v] = -1;
                }
            }
        }
    }
}

void MainLoop()
{
    RandomTree(50);
    PrepareBacktrack(0);
    printf("commencing backtrack\n");

    /* clock_t now = clock(); */
    try
    {
        Backtrack(2*n - 2);
    }
    catch(int)
    {}

    /* clock_t delta = clock() - now; */
    /* printf("%f\n", (float)delta / CLOCKS_PER_SEC); */
}

sig_atomic_t loop = false;
void SignalHandler(int)
{
    loop = 0;
}

int main()
{
    do
    {
        MainLoop();
    }
    while (loop);
}
