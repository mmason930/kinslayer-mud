#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
map_test_dir=$(mktemp -d)
trap 'rm -rf "$map_test_dir"' EXIT HUP INT TERM
# Compile the production method, not a second implementation of the traversal.
# The rest of Descriptor depends on the full game and its database connection.
python3 - "$map_test_dir/mini-map-under-test.inc" <<'PY'
from pathlib import Path
import sys
source = Path('src/Descriptor.cpp').read_text()
start = source.index('void Descriptor::sendWebSocketMiniMapCommand()')
end = source.index('std::string Descriptor::encodeWebSocketOutputCommand', start)
Path(sys.argv[1]).write_text(source[start:end])
PY
${CXX:-g++} -std=c++23 -O2 -Isrc -I"$map_test_dir" \
    tests/mini-map-visibility.cpp src/jsoncpp/json_reader.cpp \
    src/jsoncpp/json_value.cpp src/jsoncpp/json_writer.cpp \
    -o "$map_test_dir/mini-map-visibility"
"$map_test_dir/mini-map-visibility"
