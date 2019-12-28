#ifndef GLOBALS_INCLUDED
#define GLOBALS_INCLUDED

#include <vector>

const int MAX_VERTICES = 100;
const int USED_CAPACITY = 2*MAX_VERTICES;
const int SEED = 35;

extern int n;
extern std::vector<int> graph[MAX_VERTICES];

extern int vertexValues[MAX_VERTICES];
extern bool usedValues[USED_CAPACITY];
extern int pred[MAX_VERTICES];

#endif
