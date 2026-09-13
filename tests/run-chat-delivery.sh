#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
chat_test_dir=$(mktemp -d)
trap 'rm -rf "$chat_test_dir"' EXIT HUP INT TERM
python3 - "$chat_test_dir/chat-delivery-impl.inc" <<'PY'
from pathlib import Path
import sys
s = Path('src/comm.cpp').read_text()
a = s.index('const char *ACTNULL')
b = s.index('bool Character::LoggedIn()', a)
c = s.index('void SendChat(')
d = s.index('void Character::send(', c)
Path(sys.argv[1]).write_text(s[a:b] + s[c:d])
PY
${CXX:-g++} -std=c++23 -Wall -Wextra -Wno-unused-parameter -I"$chat_test_dir" tests/chat-delivery.cpp -o "$chat_test_dir/chat-delivery"
"$chat_test_dir/chat-delivery"
