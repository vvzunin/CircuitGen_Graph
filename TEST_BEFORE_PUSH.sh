#!/bin/bash
cmake "$@" --preset=ci-ubuntu
cmake --build --preset=ci-ubuntu -t format-fix -j $(nproc)
cmake --build --preset=ci-ubuntu -j $(nproc)
ctest --preset=ci-ubuntu -j $(nproc)