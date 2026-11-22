#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
export PATH="$PATH:/usr/local/cuda/bin"
make "$@"
