#!/bin/sh
# Run in the game development container with DB_HOSTNAME/USERNAME/PASSWORD/SCHEMA.
# Only constant SELECTs are sent to MySQL; no player data or schema changes.
set -eu
cd "$(dirname "$0")/.."
query_test_dir=$(mktemp -d)
trap 'rm -rf "$query_test_dir"' EXIT HUP INT TERM
${CXX:-g++} -std=c++23 -O2 -g ${JS_TEST_CXXFLAGS:-} \
    $(pkg-config --cflags mozjs-153) \
    tests/js-query.cpp src/js/flusspferd.cpp \
    src/js/JSQuery.cpp src/js/JSQuery_bindings.cpp \
    src/js/JSRow.cpp src/js/JSRow_bindings.cpp \
    -o "$query_test_dir/js-query" \
    -lsqlDatabase -lmysqlclient $(pkg-config --libs mozjs-153) -pthread
KINSLAYER_DISABLE_JIT=0 timeout 60 "$query_test_dir/js-query"
KINSLAYER_DISABLE_JIT=1 timeout 60 "$query_test_dir/js-query"
