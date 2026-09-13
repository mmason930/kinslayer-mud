#!/bin/sh
# Run in the Linux game development container. No game or database is started.
set -eu
cd "$(dirname "$0")/.."
portal_test_dir=$(mktemp -d)
trap 'rm -rf "$portal_test_dir"' EXIT HUP INT TERM
portal_source_dir=${PORTAL_TEST_SOURCE_DIR:-src}
${CXX:-g++} -std=c++23 -O1 -g -fsanitize=address,undefined \
    -fno-omit-frame-pointer -ffunction-sections -fdata-sections \
    $(pkg-config --cflags mozjs-153) -I"$portal_source_dir" \
    tests/player-portal-lifecycle.cpp \
    "$portal_source_dir/playerportal/PlayerPortalServer.cpp" \
    "$portal_source_dir/playerportal/PlayerPortalDescriptor.cpp" \
    "$portal_source_dir/playerportal/PlayerPortalDescriptorStatus.cpp" \
    "$portal_source_dir/ku/kuListener.cpp" "$portal_source_dir/ku/kuDescriptor.cpp" \
    "$portal_source_dir/jsoncpp/json_reader.cpp" \
    "$portal_source_dir/jsoncpp/json_value.cpp" "$portal_source_dir/jsoncpp/json_writer.cpp" \
    -Wl,--gc-sections -o "$portal_test_dir/player-portal-lifecycle"
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 \
    timeout 30 "$portal_test_dir/player-portal-lifecycle"
