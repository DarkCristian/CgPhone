#!/usr/bin/env python3
"""Keep the three Qt advisory entry points outside CgPhone Free."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PATTERNS = {
    "CVE-2026-78253": ("QXmlStreamReader", "readElementText("),
    "CVE-2026-79616": ("Context2D.path", "PathSvg.path", "QQuickSvgParser"),
    "CVE-2026-76151": ("QNetworkAccessManager",),
}

findings = []
for directory, extensions in (("src", {".cpp", ".h"}), ("qml", {".qml"})):
    for path in (ROOT / directory).rglob("*"):
        if path.suffix not in extensions:
            continue
        contents = path.read_text(encoding="utf-8")
        for advisory, patterns in PATTERNS.items():
            for pattern in patterns:
                if pattern in contents:
                    findings.append(f"{path.relative_to(ROOT)}: {advisory}: {pattern}")

if findings:
    raise SystemExit("Review Qt security advisory entry points before shipping:\n" + "\n".join(findings))
print("Qt advisory call-site guard: PASS (no affected APIs in CgPhone source/QML)")
