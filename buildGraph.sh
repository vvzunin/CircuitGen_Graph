#!/bin/bash
cmake -S . -B ./build -G "Unix Makefiles"
cmake --build ./build -j $(nproc)