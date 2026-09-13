#!/bin/sh
# Run in the game development container, or a host with the same dependencies.
set -eu
cd "$(dirname "$0")/.."
js_test_dir=$(mktemp -d)
trap 'rm -rf "$js_test_dir"' EXIT HUP INT TERM
${CXX:-g++} -std=c++23 -O2 -g ${JS_TEST_CXXFLAGS:-} \
    $(pkg-config --cflags mozjs-153) \
    tests/js-jit.cpp src/js/flusspferd.cpp \
    -o "$js_test_dir/js-jit" \
    $(pkg-config --libs mozjs-153) -pthread
KINSLAYER_DISABLE_JIT=0 timeout 60 "$js_test_dir/js-jit"
KINSLAYER_DISABLE_JIT=1 timeout 60 "$js_test_dir/js-jit"
