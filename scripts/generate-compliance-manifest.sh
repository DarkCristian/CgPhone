#!/usr/bin/env bash
set -euo pipefail

root_dir="${1:?Indicá la raíz del repositorio}"
portable_dir="${2:?Indicá la carpeta portable}"
pj_dir="${3:?Indicá la carpeta de PJPROJECT}"
compliance_dir="$portable_dir/COMPLIANCE"

mkdir -p "$compliance_dir"

{
  echo "CgPhone build information"
  echo "UTC: $(date -u +'%Y-%m-%dT%H:%M:%SZ')"
  echo "Git commit: $(git -C "$root_dir" rev-parse HEAD 2>/dev/null || echo unknown)"
  echo "CMake: $(cmake --version | sed -n '1p')"
  echo "Compiler: $(c++ --version | sed -n '1p')"
} > "$compliance_dir/BUILD-INFO.txt"

{
  echo "PJPROJECT commit: $(git -C "$pj_dir" rev-parse HEAD 2>/dev/null || echo unknown)"
  echo "PJPROJECT describe: $(git -C "$pj_dir" describe --always --tags 2>/dev/null || echo unknown)"
  echo
  pacman -Q | grep -E '^(mingw-w64-x86_64-(qt6|ffmpeg|openssl|lame|opus|opencore-amr|gcc|cmake|ninja)|git |make )' || true
} > "$compliance_dir/DEPENDENCY-VERSIONS.txt"

(
  cd "$portable_dir"
  find . -type f ! -path './COMPLIANCE/SHA256SUMS.txt' -print0 |
    sort -z |
    xargs -0 sha256sum
) > "$compliance_dir/SHA256SUMS.txt"

