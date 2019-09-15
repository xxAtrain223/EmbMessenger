#!/usr/bin/env bash
set -e
mkdir build
cd build

cmake -DEmbMessenger_ENABLE_TESTING:BOOL=ON ..
