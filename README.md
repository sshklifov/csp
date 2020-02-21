# Gracious tree labeling

## Документация

В doc/csp.pdf.

Интересни сорс файлове са Solver.cpp, SolverUtil.cpp и Main.cpp.
Евентуално и getcertainty.py, RandomForest.ipynb.

## Компилация

```bash
./build.sh
```

Използват се два важни макроса там:

1. **MAX_SECONDS** ограничава времето за търсене с alarm(2). Ако не се дефинира,
не се слага аларма.
2. **HAVE_PYTHON** използва machine learning. Ако не се дефинира, се използва
апроксимация.
3. **NDEBUG** е важно да се остави дефинирано.
