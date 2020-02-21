#ifdef HAVE_PYTHON
    #define PY_SSIZE_T_CLEAN
    #include <Python.h>
#endif

#include "Solver.h"

int main(int argc, char* argv[])
{
    // Initialize python environment
#ifdef HAVE_PYTHON
    wchar_t* program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);
    Py_Initialize();
#endif

    // Perform tests
    int loop = 100;
    do {
        int n = 70;
        Solver solver(n);
        solver.Search();
    } while (loop-- > 0);

    // Finish using python environment
#ifdef HAVE_PYTHON
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
#endif

    return 0;
}
