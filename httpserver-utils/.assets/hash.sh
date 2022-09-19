#!/usr/bin/env bash
set -eou pipefail
cd "$(cd $(dirname "${BASH_SOURCE[0]}") && pwd)"
find ../.assets -type f|xargs md5sum|md5sum |cut -d' ' -f1
