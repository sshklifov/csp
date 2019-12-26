#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cassert>
#include <random>
#include <memory>

std::vector<int>* ReadTree(FILE* fp, int* n)
{
    fscanf(fp, "%d", n);
    std::unique_ptr<std::vector<int>[]> graph (new std::vector<int>[*n]);
    
    int v1, v2;
    while (fscanf(fp, "%d %d", &v1, &v2) == 2)
    {
        if (v1 > v2) std::swap(v1, v2);
        graph[v1].push_back(v2);
    }

    assert(!ferror(fp));
    return graph.release();
}

void PrintTree(std::vector<int>* graph, int n)
{
    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            printf("%d %d\n", u, v);
        }
    }
}

bool HasEdge(std::vector<int>* graph, int n, int v1, int v2)
{
    for (int v : graph[v1])
    {
        if (v == v2) return true;
    }
    return false;
}

int PathCompression(std::vector<int>& pred, int i)
{
    if (pred[i] == i) return i;
    return pred[i] = PathCompression(pred, pred[i]);
}

std::vector<int>* RandomTree(int n)
{
    std::default_random_engine eng;
    std::uniform_int_distribution<int> uni(0, n-1);

    std::unique_ptr<std::vector<int>[]> graph;
    graph.reset(new std::vector<int>[n]);

    std::vector<int> pred(n);
    for (int i = 0; i < n; ++i) pred[i] = i;
    std::vector<int> rank(n, 1);

    int edges = n-1;
    while (edges > 0)
    {
        int v1 = uni(eng);
        int v2 = uni(eng);

        int parent1 = PathCompression(pred, v1);
        int parent2 = PathCompression(pred, v2);
        if (parent1 != parent2)
        {
            if (v2 < v1) std::swap(v1, v2);
            graph[v1].push_back(v2);
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
    }
    return graph.release();
}

bool CheckAssignment(const std::vector<int>* graph, int n, const int* values)
{
    assert(n < 30);
    int used[100] = {};

    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            int diff = abs(values[u] - values[v]);
            if (used[diff]) return false;
            else used[diff] = true;
        }
    }

    return true;
}

void BruteForce(const std::vector<int>* graph, int n)
{
    assert(n < 30);
    int values[30];
    for (int i = 0; i < n; ++i)
    {
        values[i] = 2*i + 1;
    }
    do
    {
        if (CheckAssignment(graph, n, values))
        {
            printf("Solution\n");
            for (int u = 0; u < n; ++u)
            {
                for (int v : graph[u])
                {
                    printf("%d %d\n", values[u], values[v]);
                }
            }
            return;
        }
    }
    while (std::next_permutation(values, values+n));

    printf("No solution\n");
}

int main()
{
    int n = 9;
    /* std::vector<int>* graph = ReadTree(stdin, &n); */
    std::vector<int>* graph = RandomTree(n);
    PrintTree(graph, n);
    BruteForce(graph, n);
    /* printf("%d\n", n); */
}
