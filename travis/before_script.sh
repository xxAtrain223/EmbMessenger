#!/usr/bin/env bash
set -e
mkdir build
cd build

cmake -DEMB_ENABLE_TESTING:BOOL=ON ..