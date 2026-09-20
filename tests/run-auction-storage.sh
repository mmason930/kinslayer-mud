#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
auction_test_dir=$(mktemp -d)
trap 'rm -rf "$auction_test_dir"' EXIT HUP INT TERM
python3 - "$auction_test_dir/auction-storage-under-test.inc" <<'PY'
from pathlib import Path
import sys
source = Path('src/js/JSObject.cpp').read_text()
start = source.index('bool JSObject::storeAndExtract(')
end = source.index('bool JSObject::canOpen()', start)
Path(sys.argv[1]).write_text(source[start:end])
PY
${CXX:-g++} -std=c++23 -O2 -Wall -Wextra -Werror -I"$auction_test_dir" \
    tests/auction-storage.cpp -o "$auction_test_dir/auction-storage"
"$auction_test_dir/auction-storage"
