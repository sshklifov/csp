#include <algorithm>
#include <csignal>

#include "Solver.h"

void MainLoop()
{
    int n = 13;
    Solver solver(n);

    std::vector<int> roots(n);
    for (int i = 0; i < n; ++i) roots[i] = i;
    std::sort(roots.begin(), roots.end(),
        [&solver](int a, int b)
        {
            // TODO
            return solver.graph[a].size() > solver.graph[b].size();
        });

    int root = roots[0];
    solver.RootTree(root);
    solver.Search();
}

sig_atomic_t loop = 1;
void SignalHandler(int)
{
    loop = 0;
}

int main()
{
    signal(SIGINT, SignalHandler);

    do
    {
        MainLoop();
    }
    while (loop);
    return 0;
}

/* for (int i = 0; i < MAX_VERTICES; ++i) */
/* { */
/*     valueDistribution[i].resize(USED_CAPACITY, 0); */
/* } */
