#!/usr/bin/env bash
set -euo pipefail
app_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
cd "$app_dir"
if [[ -x dist/level-editor ]]; then exec ./dist/level-editor "$@"; fi
if [[ ! -d node_modules ]]; then npm ci --no-audit --no-fund; fi
if [[ ! -f build/server.cjs ]] || find backend frontend shared scripts config -type f -newer build/server.cjs -print -quit | grep -q .; then npm run build; fi
exec node build/server.cjs "$@"
