#!/usr/bin/env bash
set -eou pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODE="${@:-windows}"
DIR="$(pwd)"

INTERVAL=2
LF=$(mktemp)

while :; do
	cd $DIR/../.
	passh ./build/darwin-ls/dls $MODE >$LF || { sleep 1 && continue; }
	clear
	cat $LF
	sleep $INTERVAL
done
