#!/bin/sh
# Isolated Linux tests: no game, production connections, or database writes.
set -eu
cd "$(dirname "$0")/.."
crash_test_dir=$(mktemp -d)
trap 'rm -rf "$crash_test_dir"' EXIT HUP INT TERM
${CXX:-g++} -std=c++23 -O1 -g -fsanitize=address,undefined \
    -fno-omit-frame-pointer -ffunction-sections -fdata-sections -Isrc \
    tests/crash-regressions.cpp src/jsoncpp/json_reader.cpp \
    src/jsoncpp/json_value.cpp src/jsoncpp/json_writer.cpp \
    src/ku/kuListener.cpp src/ku/kuDescriptor.cpp src/ku/kuClient.cpp \
    -Wl,--gc-sections -o "$crash_test_dir/crash-regressions"
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 \
    timeout 30 "$crash_test_dir/crash-regressions"
python3 - "$crash_test_dir/item-display-under-test.inc" <<'PY'
from pathlib import Path
import sys
objects = Path('src/js/JSObject.cpp').read_text()
objects = objects[objects.index('int JSObject::getCount()'):objects.index('bool JSObject::getIsCorpse()')]
shops = Path('src/shop.cpp').read_text()
start = shops.index('char *list_object(Character *ch, Object *obj, int cnt, int index, int shop_nr)\n{')
end = shops.index('\nint ok_shop_room', start)
Path(sys.argv[1]).write_text(objects + shops[start:end])
PY
${CXX:-g++} -std=c++23 -O1 -g -fsanitize=address,undefined \
    -fno-omit-frame-pointer -I"$crash_test_dir" \
    tests/item-display-regressions.cpp -o "$crash_test_dir/item-display-regressions"
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1 \
    timeout 30 "$crash_test_dir/item-display-regressions"
