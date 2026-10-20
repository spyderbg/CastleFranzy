#!/usr/bin/env bash
set -euo pipefail
app_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cd "$app_dir"
if [[ -x dist/auroramx ]]; then exec ./dist/auroramx "$@"; fi
if [[ ! -d node_modules ]]; then npm ci --no-audit --no-fund; fi
if [[ ! -f build/server.cjs ]]; then npm run build; fi
exec node build/server.cjs "$@"
