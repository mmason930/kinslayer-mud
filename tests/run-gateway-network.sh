#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
test_dir=$(mktemp -d)
trap 'rm -rf "$test_dir"' EXIT HUP INT TERM
${CXX:-g++} -std=c++23 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
    -ffunction-sections -fdata-sections -Isrc tests/gateway-network.cpp \
    src/websocket/WebSocketDataFrame.cpp src/ku/kuClient.cpp src/ku/kuDescriptor.cpp \
    src/ku/kuListener.cpp -Wl,--gc-sections -lcrypto -o "$test_dir/gateway-network"
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 timeout 30 "$test_dir/gateway-network"
