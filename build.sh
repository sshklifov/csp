#!/bin/bash

g++ -DHAVE_PYTHON -DMAX_SECONDS=5 -DNDEBUG -O2 $(pkg-config --cflags --libs python3-embed) *.cpp -o csp

# Compiling without python:
# g++ -DMAX_SECONDS=5 -DNDEBUG -O2 $(pkg-config --cflags --libs python3-embed) *.cpp -o csp
