#include <algorithm>
#include <csignal>
#include <ctime>
#include <cstdio>
#include <unistd.h>

#include "Solver.h"

/* sig_atomic_t loop = 1; */
/* void SignalHandler(int) */
/* { */
/*     loop = 0; */
/* } */

void MainLoop()
{
    int n = 71;
    Solver solver(n);
    if(solver.Search())
    {
        printf("Found\n");
    }
    else
    {
        printf("Not found\n");
    }
}

int main()
{
    /* signal(SIGALRM, SignalHandler); */

    do
    {
        MainLoop();
    }
    while (1);
    return 0;
}

/* for (int i = 0; i < MAX_VERTICES; ++i) */
/* { */
/*     valueDistribution[i].resize(USED_CAPACITY, 0); */
/* } */
