#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cassert>
#include <random>
#include <memory>

#include <signal.h>

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

void PrintTree(const std::vector<int>* graph, int n)
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

int seed = 53;
std::default_random_engine eng(seed);

std::vector<int>* RandomTree(int n)
{
    std::uniform_int_distribution<int> uni(0, n-1);

    std::unique_ptr<std::vector<int>[]> graph;
    graph.reset(new std::vector<int>[n]);

    std::vector<int> pred(n);
    for (int i = 0; i < n; ++i) pred[i] = i;
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

        v1 = v2;
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
    int root = 0;
    while (graph[root].size() > 0)
    {
        root = graph[root].front();
    }

    assert(n < 30);
    int values[30];
    for (int i = 0; i < n; ++i)
    {
        values[i] = 2*i + 1;
    }

    int nsolutions = 0;
    do
    {
        if (CheckAssignment(graph, n, values))
        {
            ++nsolutions;
            /* printf("Solution\n"); */
            /* for (int u = 0; u < n; ++u) */
            /* { */
            /*     for (int v : graph[u]) */
            /*     { */
            /*         printf("%d %d\n", */
            /*                 values[u], */
            /*                 values[v]); */
            /*     } */
            /* } */
            /* return; */
        }
    }
    while (std::next_permutation(values, values+n));

    /* printf("No solution\n"); */
    /* PrintTree(graph, n); */
    /* printf("using seed: %d\n", seed); */
    printf("%d\n", nsolutions);
}

/* void CountDegree(std::vector<int>* graph, int n) */
/* { */
/*     for (int i = 0; i < n; ++i) */
/*     { */
/*     } */
/* } */

sig_atomic_t loop = 1;
void FlushStuff(int)
{
    fflush(stdout);
    loop = 0;
}

int main()
{
    signal(SIGINT, FlushStuff);

    int n = 10;
    /* std::vector<int>* graph = ReadTree(stdin, &n); */
    /* seed = time(NULL); */
    while (loop)
    {

        std::vector<int>* graph = RandomTree(n);
        BruteForce(graph, n);
    }
    /* seed = 1577389239; */
    /* PrintTree(graph, n); */
}
