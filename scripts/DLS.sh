#!/usr/bin/env bash
set -eou pipefail
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODE=${1:-windows}
DIR="$(pwd)"

INTERVAL=2
LF=.o.$MODE

while :; do
	cd $DIR/../.
	passh ./build-muon/darwin-ls/dls $MODE >$LF
	clear
	cat $LF
	sleep $INTERVAL
done
