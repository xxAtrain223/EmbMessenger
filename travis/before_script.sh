#!/usr/bin/env bash
set -e
mkdir build
cd build

cmake -DENABLE_TESTING:BOOL=ON ..