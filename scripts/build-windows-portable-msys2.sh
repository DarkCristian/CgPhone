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
# windeployqt deploys Qt modules but the CMake-driven invocation used by this
# workflow does not copy the MinGW runtime DLLs. Keep them beside CgPhone.exe;
# Windows does not search the portable's parent directories for these files.
mingw_runtime_dlls=(
  libstdc++-6.dll
  libgcc_s_seh-1.dll
  libwinpthread-1.dll
  libssp-0.dll
)
for runtime_dll in "${mingw_runtime_dlls[@]}"; do
  runtime_path="/mingw64/bin/$runtime_dll"
  if [[ -f "$runtime_path" ]]; then
    cp -f "$runtime_path" "$portable_dir/bin/"
  elif [[ "$runtime_dll" != "libssp-0.dll" ]]; then
    echo "Falta el runtime obligatorio de MinGW: $runtime_path" >&2
    exit 1
  fi
done

# Qt Multimedia backends are loaded dynamically, so they never appear in the
# dependency tree of CgPhone.exe. Ensure that the backend exists in the exact
# plugin tree referenced by qt.conf before collecting DLL dependencies.
qt_multimedia_plugins="/mingw64/share/qt6/plugins/multimedia"
portable_plugins="$portable_dir/share/qt6/plugins"
if [[ ! -d "$qt_multimedia_plugins" ]]; then
  echo "MSYS2 no instaló ningún backend de Qt Multimedia" >&2
  exit 1
fi
mkdir -p "$portable_plugins/multimedia"
cp -Rf "$qt_multimedia_plugins/"* "$portable_plugins/multimedia/"

# Scan only the executable and runtime-loaded multimedia backends. Scanning
# every Qt/QML DLL recursively is redundant (windeployqt already deployed
# those modules) and turns this step into hundreds of repeated dependency
# traversals. ntldd -R already follows transitive dependencies in one pass.
dependency_report="$portable_dir/DEPENDENCIES.txt"
scan_targets=("$exe_path")
while IFS= read -r plugin_dll; do
  scan_targets+=("$plugin_dll")
done < <(find "$portable_plugins/multimedia" -type f -iname '*.dll')

: > "$dependency_report"
for binary in "${scan_targets[@]}"; do
  ntldd -R "$binary" 2>/dev/null || true
done | tee "$dependency_report"

while IFS= read -r dependency_line; do
  dependency_path="$(printf '%s\n' "$dependency_line" | sed -n 's/^.*=>[[:space:]]*\(.*\)[[:space:]]*(0x[[:xdigit:]]*)$/\1/p')"
  dependency_path="${dependency_path#"${dependency_path%%[![:space:]]*}"}"
  dependency_path="${dependency_path%"${dependency_path##*[![:space:]]}"}"
  [[ -n "$dependency_path" ]] || continue
  dependency_unix_path="$(cygpath -u "$dependency_path" 2>/dev/null || true)"
  if [[ "$dependency_unix_path" == */mingw64/bin/*.dll ]] && [[ -f "$dependency_unix_path" ]]; then
    cp -f "$dependency_unix_path" "$portable_dir/bin/"
  fi
done < "$dependency_report"

# Re-scan using the portable directory first. Any unresolved non-system DLL
# means the artifact is incomplete and must not be uploaded.
export PATH="$portable_dir/bin:/mingw64/bin:$PATH"
validation_report="$portable_dir/DEPENDENCIES-VALIDATED.txt"
: > "$validation_report"
for binary in "${scan_targets[@]}"; do
  ntldd -R "$binary" 2>/dev/null || true
done | tee "$validation_report"
# ntldd reports Windows API-set contracts (api-ms-*/ext-ms-*) and optional OS
# components as "not found" even though the Windows loader resolves them. Do
# not treat those as redistributable DLL failures. The meaningful failure is a
# dependency still being resolved from the temporary MSYS2 installation.
if grep -Eiq '[\\/]+mingw64[\\/]+bin[\\/]' "$validation_report"; then
  echo "El portable todavía depende de DLL externas de MSYS2" >&2
  exit 1
fi

required_transitive_dlls=(
  libopus-0.dll
  libcrypto-3-x64.dll
  libopencore-amrnb-0.dll
  libssl-3-x64.dll
)
for required_dll in "${required_transitive_dlls[@]}"; do
  if [[ ! -f "$portable_dir/bin/$required_dll" ]]; then
    echo "Falta una dependencia transitiva obligatoria: $required_dll" >&2
    exit 1
  fi
done

# Qt's CMake install script already ran windeployqt and created qt.conf,
# plugins and QML modules under package/. Running windeployqt a second time
# against the copied tree is redundant and fails on MSYS2 when qmlimportscanner
# cannot be started from the mixed Windows/POSIX path environment.
cp "$root_dir/PORTABLE-LEEME.txt" "$portable_dir/"
bash "$root_dir/scripts/generate-compliance-manifest.sh" "$root_dir" "$portable_dir" "$pj_dir"

pushd "$root_dir"
rm -f CgPhone-0.3.0-windows-x64-portable.zip
zip -qr CgPhone-0.3.0-windows-x64-portable.zip CgPhone-portable
popd
