#ifdef HAVE_PYTHON
    #define PY_SSIZE_T_CLEAN
    #include <Python.h>
#endif

#include "Solver.h"

int main(int argc, char* argv[])
{
#ifdef HAVE_PYTHON
    wchar_t* program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);
    Py_Initialize();
#endif

    int loop = 100;
    do {
        int n = 50;
        Solver solver(n);
        solver.Search();
    } while (loop-- > 0);

#ifdef HAVE_PYTHON
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
#endif

    return 0;
}
