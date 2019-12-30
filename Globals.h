#ifndef GLOBALS_INCLUDED
#define GLOBALS_INCLUDED

#include <vector>

const int MAX_VERTICES = 100;
const int USED_CAPACITY = 2*MAX_VERTICES;
const int SEED = 51;

extern std::vector<int> graph[MAX_VERTICES];
extern int n;

extern int nextVertex[MAX_VERTICES];
extern int pred[MAX_VERTICES];
extern int nodeCount[MAX_VERTICES];
extern int depth[MAX_VERTICES];
extern int values[MAX_VERTICES];

extern int usedValue[USED_CAPACITY];
extern int usedDiff[USED_CAPACITY];

#endif
