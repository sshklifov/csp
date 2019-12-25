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

int* conflict;
int* values;
std::pair<int,int>* diffUsed;
int* used;

void CheckAssignment(std::vector<int>* graph, int n)
{
    for (int u = 0; u < n; ++u)
    {
        for (int v = 0; v < (int)graph[u].size(); ++v)
        {
            int diff = values[u] - values[v];
            if (diffUsed[diff].first == -1)
            {
                diffUsed[diff].first = u;
                diffUsed[diff].second = v;
            }
            else
            {
                conflict[diffUsed[diff].first] = true;
                conflict[diffUsed[diff].second] = true;
                conflict[u] = true;
                conflict[v] = true;
            }
        }
    }

    for (int i = 0; i < n; ++i)
    {
        if (used[values[i]] == -1)
        {
            used[values[i]] = i;
        }
        else
        {
            conflict[i] = true;
            conflict[used[values[i]]] = true;
        }
    }
}

int main()
{
    int n;
    std::vector<int>* graph = ReadTree(stdin, &n);
    PrintTree(graph, n);
}
