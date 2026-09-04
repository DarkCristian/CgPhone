#!/usr/bin/env python3
"""Generate a deterministic SPDX 2.3 SBOM from the packaged runtime tree.

The script intentionally uses only Python's standard library so it can run in
GitHub Actions without installing a third-party generator. It inventories the
files that are actually shipped and records the exact dependency versions
captured by generate-compliance-manifest.sh.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from datetime import datetime, timezone
from pathlib import Path


PACKAGE_PREFIXES = (
    "mingw-w64-x86_64-qt6-",
    "mingw-w64-x86_64-ffmpeg",
    "mingw-w64-x86_64-openssl",
    "mingw-w64-x86_64-lame",
    "mingw-w64-x86_64-opus",
    "mingw-w64-x86_64-opencore-amr",
    "mingw-w64-x86_64-gcc",
)


def digest(path: Path, algorithm: str) -> str:
    value = hashlib.new(algorithm)
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def spdx_id(value: str) -> str:
    cleaned = re.sub(r"[^A-Za-z0-9.-]+", "-", value).strip("-")
    return f"SPDXRef-{cleaned or 'item'}"


def file_types(path: Path) -> list[str]:
    suffix = path.suffix.lower()
    if suffix in {".exe", ".dll"}:
        return ["BINARY"]
    if suffix in {".txt", ".json", ".conf", ".ini", ".qml", ".md"}:
        return ["TEXT"]
    if suffix in {".wav", ".mp3"}:
        return ["AUDIO"]
    if suffix in {".png", ".ico", ".svg", ".jpg", ".jpeg"}:
        return ["IMAGE"]
    return ["OTHER"]


def dependencies(path: Path) -> list[tuple[str, str]]:
    result: list[tuple[str, str]] = [("PJPROJECT", "2.17")]
    if not path.exists():
        return result
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = line.strip()
        if not line or line.startswith("PJPROJECT"):
            continue
        parts = line.rsplit(" ", 1)
        if len(parts) != 2:
            continue
        name, version = parts
        if name.startswith(PACKAGE_PREFIXES):
            result.append((name, version))
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("runtime", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--version", default="0.3.1-beta.35")
    parser.add_argument("--commit", default="NOASSERTION")
    parser.add_argument("--portable-sha256")
    parser.add_argument("--installer-sha256")
    args = parser.parse_args()

    runtime = args.runtime.resolve()
    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    excluded = {output}

    files = []
    relationships = []
    verification_hashes = []
    root_id = "SPDXRef-Package-CgPhone"

    for index, path in enumerate(sorted(p for p in runtime.rglob("*") if p.is_file())):
        resolved = path.resolve()
        if resolved in excluded or path.name == "SHA256SUMS.txt":
            continue
        relative = path.relative_to(runtime).as_posix()
        sha1 = digest(path, "sha1")
        sha256 = digest(path, "sha256")
        verification_hashes.append(sha1)
        file_id = f"SPDXRef-File-{index:05d}"
        files.append({
            "SPDXID": file_id,
            "fileName": f"./{relative}",
            "checksums": [
                {"algorithm": "SHA1", "checksumValue": sha1},
                {"algorithm": "SHA256", "checksumValue": sha256},
            ],
            "fileTypes": file_types(path),
            "licenseConcluded": "NOASSERTION",
            "copyrightText": "NOASSERTION",
        })
        relationships.append({
            "spdxElementId": root_id,
            "relationshipType": "CONTAINS",
            "relatedSpdxElement": file_id,
        })

    package_checksums = []
    if args.portable_sha256:
        package_checksums.append({"algorithm": "SHA256", "checksumValue": args.portable_sha256})

    packages = [{
        "SPDXID": root_id,
        "name": "CgPhone Free Windows x64 portable",
        "versionInfo": args.version,
        "downloadLocation": f"https://github.com/DarkCristian/CgPhone/releases/tag/v{args.version}",
        "filesAnalyzed": True,
        "packageVerificationCode": {
            "packageVerificationCodeValue": hashlib.sha1(
                "".join(sorted(verification_hashes)).encode("ascii")
            ).hexdigest()
        },
        "checksums": package_checksums,
        "licenseConcluded": "NOASSERTION",
        "licenseDeclared": "NOASSERTION",
        "copyrightText": "Copyright 2026 Cristian Carroccio",
        "supplier": "Person: Cristian Carroccio",
        "primaryPackagePurpose": "APPLICATION",
        "sourceInfo": f"Built from commit {args.commit}",
    }]

    installer_id = "SPDXRef-Package-CgPhone-Installer"
    if args.installer_sha256:
        packages.append({
            "SPDXID": installer_id,
            "name": "CgPhone Free Windows x64 installer",
            "versionInfo": args.version,
            "downloadLocation": f"https://github.com/DarkCristian/CgPhone/releases/tag/v{args.version}",
            "filesAnalyzed": False,
            "checksums": [{"algorithm": "SHA256", "checksumValue": args.installer_sha256}],
            "licenseConcluded": "NOASSERTION",
            "licenseDeclared": "NOASSERTION",
            "copyrightText": "Copyright 2026 Cristian Carroccio",
            "supplier": "Person: Cristian Carroccio",
            "primaryPackagePurpose": "INSTALL",
            "comment": "The installer was validated by CI to contain the same runtime tree as the portable package.",
        })
        relationships.append({
            "spdxElementId": installer_id,
            "relationshipType": "GENERATED_FROM",
            "relatedSpdxElement": root_id,
        })

    dependency_file = runtime / "COMPLIANCE" / "DEPENDENCY-VERSIONS.txt"
    for name, version in dependencies(dependency_file):
        dep_id = spdx_id(f"Package-{name}")
        packages.append({
            "SPDXID": dep_id,
            "name": name,
            "versionInfo": version,
            "downloadLocation": "NOASSERTION",
            "filesAnalyzed": False,
            "licenseConcluded": "NOASSERTION",
            "licenseDeclared": "NOASSERTION",
            "copyrightText": "NOASSERTION",
            "primaryPackagePurpose": "LIBRARY",
        })
        relationships.append({
            "spdxElementId": root_id,
            "relationshipType": "DEPENDS_ON",
            "relatedSpdxElement": dep_id,
        })

    namespace_hash = args.portable_sha256 or hashlib.sha256(
        (args.version + args.commit).encode("utf-8")
    ).hexdigest()
    document = {
        "spdxVersion": "SPDX-2.3",
        "dataLicense": "CC0-1.0",
        "SPDXID": "SPDXRef-DOCUMENT",
        "name": f"CgPhone-{args.version}-Windows-x64",
        "documentNamespace": f"https://github.com/DarkCristian/CgPhone/sbom/{args.version}/{namespace_hash}",
        "creationInfo": {
            "created": datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z"),
            "creators": ["Tool: CgPhone generate-spdx-sbom.py", "Person: Cristian Carroccio"],
            "licenseListVersion": "3.26",
        },
        "documentDescribes": [root_id, *([installer_id] if args.installer_sha256 else [])],
        "packages": packages,
        "files": files,
        "relationships": relationships,
        "annotations": [{
            "annotationDate": datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z"),
            "annotationType": "OTHER",
            "annotator": "Tool: CgPhone generate-spdx-sbom.py",
            "comment": "Generated from the final packaged runtime. NOASSERTION license fields require review against distributed third-party license texts.",
        }],
    }
    output.write_text(json.dumps(document, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
