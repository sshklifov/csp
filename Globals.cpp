#include "Globals.h"

std::vector<int> graph[MAX_VERTICES];
int n;

int nextVertex[MAX_VERTICES];
int pred[MAX_VERTICES];
int nodeCount[MAX_VERTICES];
int depth[MAX_VERTICES];
int values[MAX_VERTICES];

int usedValue[USED_CAPACITY];
int usedDiff[USED_CAPACITY];
