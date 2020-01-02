#include "Solver.h"

int main()
{
    int loop = 20;
    do
    {
        int n = 71;
        Solver solver(n);

        solver.Search();
    }
    while (loop --> 0);

    return 0;
}
