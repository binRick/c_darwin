#!/usr/bin/env bash
set -eou pipefail
cd $(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
EXEC="${EXEC:-./keylogger}"
cd ../

parse_json() {
	yaml2json <keybinds.yaml | jq -Mr >keybinds.json
}

of=.stdout
ef=.stderr

killall keylogger 2>/dev/null || true
[[ -f bin/keylogger ]] && unlink bin/keylogger

make cc-bat
#parse_json
#vterm-ctrl reset
off() {
	vterm-ctrl altscreen off
}
vterm-ctrl altscreen on
trap off EXIT

eval "$EXEC"
