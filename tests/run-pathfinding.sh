#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
path_test_dir=$(mktemp -d)
trap 'rm -rf "$path_test_dir"' EXIT HUP INT TERM
python3 - "$path_test_dir/pathfinding-under-test.inc" <<'PY'
from pathlib import Path
import sys
source = Path('src/graph.cpp').read_text()
start = source.index('// Pathfinding runs synchronously')
end = source.index('/********************************************************\n* Functions and Commands', start)
Path(sys.argv[1]).write_text(source[start:end])
PY
${CXX:-g++} -std=c++23 -O2 -g -Wall -Wextra ${PATH_TEST_CXXFLAGS:-} \
    -Isrc -I"$path_test_dir" tests/pathfinding.cpp -o "$path_test_dir/pathfinding"
"$path_test_dir/pathfinding"
