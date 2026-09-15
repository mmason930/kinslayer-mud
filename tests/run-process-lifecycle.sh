#!/bin/sh
# Run in a Linux development container; no game or database is started.
set -eu
cd "$(dirname "$0")/.."
process_test_dir=$(mktemp -d)
trap 'rm -rf "$process_test_dir"' EXIT HUP INT TERM
${CXX:-g++} -std=c++23 -O2 -g -Wall -Wextra -Werror \
    -Isrc tests/process-lifecycle.cpp src/SystemUtil.cpp \
    -o "$process_test_dir/process-lifecycle"
"$process_test_dir/process-lifecycle"
