#!/bin/bash
"/snap/cmake/1403/bin/cmake" -S ./ -B ./Build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++-19
"/snap/cmake/1403/bin/cmake" --build ./Build --config=Release