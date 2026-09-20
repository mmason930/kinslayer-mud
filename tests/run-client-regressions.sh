#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
client_test_dir=$(mktemp -d)
trap 'rm -rf "$client_test_dir"' EXIT HUP INT TERM
python3 - "$client_test_dir" <<'PY'
from pathlib import Path
import sys
out = Path(sys.argv[1])
source = Path('src/PvalManager.cpp').read_text()
key = source[source.index('static std::string makeFullKey'):source.index('void PvalManager::preBoot')]
methods = source[source.index('std::string PvalManager::getPval'):source.index('void PvalManager::deletePval')]
(out / 'pval-under-test.inc').write_text(key + methods)
source = Path('src/items/ObjectSelection.h').read_text()
(out / 'client-visibility-under-test.inc').write_text(source[source.index('inline bool visibleOwnedItem'):source.index('// An exact selector')])
PY
for test in pval-persistence client-item-visibility; do
    ${CXX:-g++} -std=c++23 -O1 -g -Isrc -I"$client_test_dir" "tests/$test.cpp" -o "$client_test_dir/$test"
    "$client_test_dir/$test"
done
