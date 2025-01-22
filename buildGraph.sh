#!/bin/bash
cmake -S . -B ./build -G "Unix Makefiles" -DENABLE_OPTIMIZATIONS_GRAPH=ON
cmake --build ./build -j $(nproc)