#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <random>
#include <memory>
#include <cstring>
#include <stack>
#include <queue>

#define MAX_VERTICES 50

int nextVertex[MAX_VERTICES];
int values[MAX_VERTICES];
int n;
std::vector<int> graph[MAX_VERTICES];
std::vector<int> adj[MAX_VERTICES];

bool CheckAssignment()
{
    const int USED_CAPACITY = 2*MAX_VERTICES;
    int used[USED_CAPACITY] = {};

    for (int u = 0; u < n; ++u)
    {
        for (int v : graph[u])
        {
            int diff = abs(values[u] - values[v]);
            if (used[diff]) return false;
            else used[diff] = true;
        }
    }

    memset(used, 0, sizeof(int) * USED_CAPACITY);
    for (int i = 0; i < n; ++i)
    {
        if (used[values[i]]) return false;
        else used[values[i]] = true;
    }
    return true;
}

int PathCompression(std::vector<int>& pred, int i)
{
    if (pred[i] == i) return i;
    return pred[i] = PathCompression(pred, pred[i]);
}

void RandomTree(int n)
{
    static std::default_random_engine eng (32);
    std::uniform_int_distribution<int> uni(0, n-1);

    ::n = n;

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

int usedValues[2*MAX_VERTICES] = {};
int usedDiff[2*MAX_VERTICES] = {};
std::stack<int> s;
void Backtrack(int i)
{
    if (i == -1)
    {
        /* exit(0); */
        /* printf("dubre\n"); */
        /* for (int i = 0; i < n; ++i) */
        /* { */
            /* printf("%d ", values[i]); */
        /* } */
        /* printf("\n"); */
        return;
    }

    for (int vidx = 0; vidx < n; ++vidx) // precompute
    {
        int newValue = 2*vidx + 1;
        if (!usedValues[newValue])
        {
            bool pushed = false;
            for (int j : adj[i])
            {
                if (values[j] >= 0)
                {
                    int diff = abs(values[j]-newValue); // values[i]
                    if (usedDiff[diff]) // precompute
                    {
                        while (s.top() != -1)
                        {
                            usedDiff[s.top()] = false;
                            s.pop();
                        }
                        goto JMP;
                    }
                    else
                    {
                        pushed = true;
                        s.push(diff);
                        usedDiff[diff] = true;
                    }
                }
            }

            if(pushed) s.push(-1);
            values[i] = newValue;
            usedValues[newValue] = true;
            Backtrack(nextVertex[i]);
            usedValues[newValue] = false;
            values[i] = -1;
            if (pushed)
            {
                assert(s.top() == -1);
                s.pop();
                while(s.top() != -1)
                {
                    usedDiff[s.top()] = false;
                    s.pop();
                }
            }
        }
        JMP:
        1;
    }
}

int main()
{
    RandomTree(11);
    for (int i = 0; i < n; ++i)
    {
        for (int j : graph[i])
        {
            adj[i].push_back(j);
            adj[j].push_back(i);
        }
    }

    for (int i = 0; i < n; ++i) nextVertex[i] = i+1;
    for (int i = 0; i < n; ++i) values[i] = -1;
    nextVertex[n-1] = -1;
    s.push(-1);

    Backtrack(0);

    return 0;
}
