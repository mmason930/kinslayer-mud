#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
timing_test_dir=$(mktemp -d)
trap 'rm -rf "$timing_test_dir"' EXIT HUP INT TERM
${CXX:-g++} -std=c++23 -O2 -g -Wall -Wextra -Werror \
    ${TIMING_TEST_CXXFLAGS:-} -Isrc tests/game-loop-timing.cpp \
    -o "$timing_test_dir/game-loop-timing"
"$timing_test_dir/game-loop-timing"
