#!/bin/bash

g++ -DHAVE_PYTHON -DMAX_SECONDS=5 -DNDEBUG -O2 $(pkg-config --cflags --libs python3-embed) *.cpp -o csp
#g++ -ggdb $(pkg-config --cflags --libs python3-embed) *.cpp -o csp
