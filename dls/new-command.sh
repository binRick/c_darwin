#!/usr/bin/env bash
set -eou pipefail
CMD=$1
cmd="cp command-template.h command-$CMD.h"
eval "$cmd"
cmd="cp command-template.c command-$CMD.c"
eval "$cmd"
cmd="gsed -i 's/TEMPLATE/$CMD/g' command-$CMD.h command-$CMD.c"
eval "$cmd"
