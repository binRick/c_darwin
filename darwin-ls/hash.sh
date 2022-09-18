#!/usr/bin/env bash
set -eou pipefail
find assets -type f|xargs md5sum|md5sum |cut -d' ' -f1
