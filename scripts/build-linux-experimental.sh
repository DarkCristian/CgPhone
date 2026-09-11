#!/usr/bin/env bash
set -euo pipefail
root="$(pwd)"
apt-get install -y --no-install-recommends qml6-module-qtquick-dialogs uuid-dev
pj_commit=a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9
mkdir -p .linux-deps dist
git clone --no-checkout https://github.com/pjsip/pjproject.git .linux-deps/pjproject
git -C .linux-deps/pjproject checkout --detach "$pj_commit"
test "$(git -C .linux-deps/pjproject rev-parse HEAD)" = "$pj_commit"
cat > .linux-deps/pjproject/pjlib/include/pj/config_site.h <<'EOF'
#define PJMEDIA_HAS_VIDEO 0
#define PJMEDIA_HAS_SRTP 0
#define PJMEDIA_SDP_NEG_MAINTAIN_REMOTE_PT_MAP 0
#define PJSUA_MAX_CALLS 2
#include <pj/config_site_sample.h>
EOF
pushd .linux-deps/pjproject
./configure --prefix="$root/.linux-deps/install" --disable-video --disable-ffmpeg --disable-sdl --disable-libyuv
make dep
make -j2
make install
popd
export PKG_CONFIG_PATH="$root/.linux-deps/install/lib/pkgconfig"
cmake -S . -B build-linux -G Ninja -DCMAKE_BUILD_TYPE=Release -DCGPHONE_WITH_PJSIP=ON -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$root/build-linux/bin" -DCMAKE_SKIP_RPATH=ON
cmake --build build-linux --parallel 2
QT_QPA_PLATFORM=offscreen ctest --test-dir build-linux --output-on-failure | tee dist/Linux-tests.txt
stage="$root/linux-stage"
install -Dm755 build-linux/bin/CgPhone "$stage/usr/bin/CgPhone"
install -Dm644 assets/app/cgphone.desktop "$stage/usr/share/applications/cgphone.desktop"
install -Dm644 assets/app/CgPhone.png "$stage/usr/share/icons/hicolor/512x512/apps/cgphone.png"
install -Dm644 docs/LINUX_EXPERIMENTAL.md "$stage/usr/share/doc/cgphone/LINUX_EXPERIMENTAL.md"
cp docs/LINUX_EXPERIMENTAL.md dist/LINUX-EXPERIMENTAL.md
install -Dm644 .linux-deps/pjproject/COPYING "$stage/usr/share/doc/cgphone/COPYING-PJPROJECT"
mkdir -p debian
cat > debian/control <<'EOF'
Source: cgphone
Section: net
Priority: optional
Maintainer: CgPhone contributors <noreply@github.com>
Standards-Version: 4.7.0

Package: cgphone
Architecture: amd64
Description: Experimental CgPhone SIP softphone
EOF
deps="$(dpkg-shlibdeps -O -e"$stage/usr/bin/CgPhone" | sed -n 's/^shlibs:Depends=//p')"
test -n "$deps"
deps="$deps, qml6-module-qtquick, qml6-module-qtquick-controls, qml6-module-qtquick-layouts, qml6-module-qtquick-window, qml6-module-qtquick-templates, qml6-module-qtquick-dialogs, qml6-module-qtqml-workerscript, qml6-module-qtmultimedia, qt6-qpa-plugins, pulseaudio-utils"
printf '%s\n' "$deps" > dist/Linux-runtime-dependencies.txt
dpkg-query -W -f='${binary:Package}\t${Version}\n' > dist/Linux-build-packages.tsv
{
  echo 'Target=Debian 13 (trixie) amd64; dynamic system Qt runtime'
  echo "CgPhoneCommit=$(git rev-parse HEAD)"
  echo "PJPROJECTCommit=$pj_commit"
  echo "BuiltAtUtc=$(date -u +%FT%TZ)"
  echo 'SIPTests=not performed; requires real PBX and audio devices'
  echo 'Antivirus=not performed for Linux; Windows Defender report applies only to Windows'
  echo 'MP3=not supported in this Linux preview; use WAV'
} > dist/Linux-build-info.txt
mkdir -p "$stage/DEBIAN"
cat > "$stage/DEBIAN/control" <<EOF
Package: cgphone
Version: 0.3.2~rc1
Section: net
Priority: optional
Architecture: amd64
Maintainer: CgPhone contributors <noreply@github.com>
Depends: $deps
Homepage: https://github.com/DarkCristian/CgPhone
Description: CgPhone Free experimental Linux SIP softphone
 Experimental Debian 13 build. SIP credentials are stored without encryption.
EOF
dpkg-deb --root-owner-group --build "$stage" dist/cgphone_0.3.2-rc.1_linux-experimental_amd64.deb
tar -cJf dist/CgPhone-0.3.2-rc.1-linux-experimental-amd64.tar.xz --exclude=DEBIAN -C "$stage" .
python3 scripts/linux-sbom.py
cd dist
sha256sum cgphone_*.deb CgPhone-*-linux-*.tar.xz CgPhone-linux-sbom.spdx.json Linux-* LINUX-EXPERIMENTAL.md > LINUX-SHA256SUMS.txt
