#!/usr/bin/env bash
# Download and unpack Ubuntu build/runtime dependencies locally, without sudo.
set -euo pipefail
project_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
cd "$project_dir"
. /etc/os-release
if [[ $ID != ubuntu || $(uname -m) != x86_64 ]]; then
    echo "This optional bootstrap supports x86_64 Ubuntu. See readme.txt for system packages." >&2
    exit 1
fi
mkdir -p .deps/linux64/apt/lists/partial .deps/linux64/apt/cache/archives/partial .deps/linux64/root
cat > .deps/linux64/apt/ubuntu.sources <<EOF
Types: deb
URIs: http://archive.ubuntu.com/ubuntu/
Suites: $VERSION_CODENAME $VERSION_CODENAME-updates
Components: main universe
Architectures: amd64
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

Types: deb
URIs: http://security.ubuntu.com/ubuntu/
Suites: $VERSION_CODENAME-security
Components: main universe
Architectures: amd64
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg
EOF
apt_options=(
    -o Debug::NoLocking=1 -o APT::Architecture=amd64 -o APT::Architectures::=amd64
    -o Dir::State::status=/dev/null
    -o "Dir::State::lists=$project_dir/.deps/linux64/apt/lists"
    -o "Dir::Cache=$project_dir/.deps/linux64/apt/cache"
    -o "Dir::Etc::sourcelist=$project_dir/.deps/linux64/apt/ubuntu.sources"
    -o Dir::Etc::sourceparts=- -o Acquire::Languages=none
)
apt-get "${apt_options[@]}" update
apt-get "${apt_options[@]}" --download-only --no-install-recommends -y install \
    libgl-dev libglu1-mesa-dev libopenal-dev libx11-dev libxxf86vm-dev libgl1-mesa-dri libpulse0
python3 - <<'PY'
from pathlib import Path
import subprocess
root = Path('.deps/linux64/root').resolve()
packages = list(Path('.deps/linux64/apt/cache/archives').glob('*.deb'))
for package in packages:
    subprocess.run(['dpkg-deb', '-x', str(package), str(root)], check=True)
print(f'Prepared {len(packages)} packages in {root}')
PY
echo 'Dependencies ready. Run: cmake --fresh --preset linux-debug && cmake --build --preset linux-debug'
