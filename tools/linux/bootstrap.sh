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
gcc_version=$(g++ -dumpversion | cut -d. -f1)
mkdir -p .deps/downloads .deps/apt/lists/partial .deps/apt/cache/archives/partial .deps/root
cat > .deps/apt/ubuntu.sources <<EOF
Types: deb
URIs: http://archive.ubuntu.com/ubuntu/
Suites: $VERSION_CODENAME $VERSION_CODENAME-updates
Components: main universe
Architectures: i386
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

Types: deb
URIs: http://security.ubuntu.com/ubuntu/
Suites: $VERSION_CODENAME-security
Components: main universe
Architectures: i386
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg
EOF
apt_options=(
    -o Debug::NoLocking=1 -o APT::Architecture=i386 -o APT::Architectures::=i386
    -o Dir::State::status=/dev/null
    -o "Dir::State::lists=$project_dir/.deps/apt/lists"
    -o "Dir::Cache=$project_dir/.deps/apt/cache"
    -o "Dir::Etc::sourcelist=$project_dir/.deps/apt/ubuntu.sources"
    -o Dir::Etc::sourceparts=- -o Acquire::Languages=none
)
apt-get "${apt_options[@]}" update
apt-get "${apt_options[@]}" --download-only --no-install-recommends -y install \
    libgl-dev libglu1-mesa-dev libopenal-dev libx11-dev libxxf86vm-dev libgl1-mesa-dri libpulse0
(
    cd .deps/downloads
    apt download "lib32gcc-$gcc_version-dev" "lib32stdc++-$gcc_version-dev" \
        libc6-dev libc6-dev-i386 linux-libc-dev libc6-i386 lib32gcc-s1 lib32stdc++6 \
        lib32asan8 lib32atomic1 lib32gomp1 lib32itm1 lib32quadmath0 lib32ubsan1
)
python3 - <<'PY'
from pathlib import Path
import subprocess
root = Path('.deps/root').resolve()
packages = list(Path('.deps/apt/cache/archives').glob('*.deb'))
packages += [p for p in Path('.deps/downloads').glob('*.deb')
             if p.name.startswith(('lib32', 'libc6-', 'linux-libc-dev_'))]
for package in packages:
    subprocess.run(['dpkg-deb', '-x', str(package), str(root)], check=True)
# The multilib linker script uses absolute system paths; redirect to the local copy.
lib = root / 'usr/lib32'
(lib / 'libc.so').write_text(
    f'OUTPUT_FORMAT(elf32-i386)\nGROUP ( {lib}/libc.so.6 {lib}/libc_nonshared.a '
    f'AS_NEEDED ( {lib}/ld-linux.so.2 ) )\n')
print(f'Prepared {len(packages)} packages in {root}')
PY
echo 'Dependencies ready. Run: cmake --preset linux-debug && cmake --build --preset linux-debug'
