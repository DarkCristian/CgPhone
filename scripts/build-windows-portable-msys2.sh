#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "$0")/.." && pwd)"
work_dir="${RUNNER_TEMP:-$root_dir/.build-deps}/cgphone"
pj_dir="$work_dir/pjproject"

mkdir -p "$work_dir"
if [[ ! -d "$pj_dir/.git" ]]; then
  git clone --depth 1 --branch 2.17 https://github.com/pjsip/pjproject.git "$pj_dir"
fi

mkdir -p "$pj_dir/pjlib/include/pj"
cat > "$pj_dir/pjlib/include/pj/config_site.h" <<'EOF'
#define PJMEDIA_HAS_VIDEO 0
#define PJSUA_MAX_CALLS 2
#include <pj/config_site_sample.h>
EOF

pushd "$pj_dir"
./configure --prefix=/mingw64 --disable-video --disable-ffmpeg --disable-sdl --disable-libyuv
make dep
make -j2
make install
popd

export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
pkg-config --modversion libpjproject

cmake -S "$root_dir" -B "$root_dir/build-windows" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCGPHONE_WITH_PJSIP=ON
cmake --build "$root_dir/build-windows" --parallel 2
ctest --test-dir "$root_dir/build-windows" --output-on-failure

rm -rf "$root_dir/package" "$root_dir/CgPhone-portable"
cmake --install "$root_dir/build-windows" --prefix "$root_dir/package"
portable_dir="$root_dir/CgPhone-portable"
mkdir -p "$portable_dir"
cp -R "$root_dir/package/"* "$portable_dir/"

exe_path="$portable_dir/bin/CgPhone.exe"
if [[ ! -f "$exe_path" ]]; then
  echo "No se generó $exe_path" >&2
  exit 1
fi
# Qt's CMake install script already ran windeployqt and created qt.conf,
# plugins and QML modules under package/. Running windeployqt a second time
# against the copied tree is redundant and fails on MSYS2 when qmlimportscanner
# cannot be started from the mixed Windows/POSIX path environment.
cp "$root_dir/PORTABLE-LEEME.txt" "$portable_dir/"

pushd "$root_dir"
rm -f CgPhone-0.2.10-windows-x64-portable.zip
zip -qr CgPhone-0.2.10-windows-x64-portable.zip CgPhone-portable
popd
