#!/usr/bin/env bash
set -e
pushd build

lcov --directory . --capture --output-file coverage.info # capture coverage info
lcov --remove coverage.info '/usr/*' --output-file coverage.info # filter out system
lcov --remove coverage.info 'ext/*' --output-file coverage.info # filter out external libraries
lcov --remove coverage.info 'test/*' --output-file coverage.info # filter out testing files

lcov --list coverage.info #debug info

bash <(curl -s https://codecov.io/bash) -t a80551a3-9f8b-496c-8002-2608aa375cda