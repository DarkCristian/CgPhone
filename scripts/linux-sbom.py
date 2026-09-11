import hashlib
import json
import pathlib
import subprocess
import uuid
from datetime import datetime, timezone

root = pathlib.Path('linux-stage')
commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], text=True).strip()
packages = []
relationships = []
for index, line in enumerate(pathlib.Path('dist/Linux-build-packages.tsv').read_text().splitlines()):
    name, version = line.split('\t', 1)
    packages.append({'name': name, 'SPDXID': f'SPDXRef-Debian-{index}', 'versionInfo': version,
                     'downloadLocation': 'NOASSERTION', 'filesAnalyzed': False,
                     'licenseConcluded': 'NOASSERTION', 'licenseDeclared': 'NOASSERTION',
                     'copyrightText': 'NOASSERTION',
                     'comment': 'Installed build environment package; not necessarily shipped in the archive.'})
packages += [
    {'name': 'CgPhone', 'SPDXID': 'SPDXRef-CgPhone', 'versionInfo': '0.3.2-rc.1',
     'downloadLocation': f'git+https://github.com/DarkCristian/CgPhone.git@{commit}',
     'filesAnalyzed': False, 'licenseConcluded': 'NOASSERTION', 'licenseDeclared': 'NOASSERTION',
     'copyrightText': 'NOASSERTION'},
    {'name': 'PJPROJECT', 'SPDXID': 'SPDXRef-PJPROJECT',
     'versionInfo': 'a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9',
     'downloadLocation': 'git+https://github.com/pjsip/pjproject.git@a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9',
     'filesAnalyzed': False, 'licenseConcluded': 'NOASSERTION', 'licenseDeclared': 'NOASSERTION',
     'copyrightText': 'NOASSERTION'}]
files = []
for index, path in enumerate(sorted(root.rglob('*'))):
    if not path.is_file() or 'DEBIAN' in path.parts:
        continue
    file_id = f'SPDXRef-File-{index}'
    files.append({'fileName': './'+path.relative_to(root).as_posix(), 'SPDXID': file_id,
                  'checksums': [{'algorithm': 'SHA256', 'checksumValue': hashlib.sha256(path.read_bytes()).hexdigest()}],
                  'licenseConcluded': 'NOASSERTION', 'copyrightText': 'NOASSERTION'})
    relationships.append({'spdxElementId': 'SPDXRef-CgPhone', 'relationshipType': 'CONTAINS', 'relatedSpdxElement': file_id})
relationships += [{'spdxElementId': 'SPDXRef-DOCUMENT', 'relationshipType': 'DESCRIBES', 'relatedSpdxElement': 'SPDXRef-CgPhone'},
                  {'spdxElementId': 'SPDXRef-CgPhone', 'relationshipType': 'DEPENDS_ON', 'relatedSpdxElement': 'SPDXRef-PJPROJECT'}]
doc = {'spdxVersion': 'SPDX-2.3', 'dataLicense': 'CC0-1.0', 'SPDXID': 'SPDXRef-DOCUMENT',
       'name': 'CgPhone Linux experimental package and build inventory',
       'documentNamespace': 'https://github.com/DarkCristian/CgPhone/sbom/'+str(uuid.uuid4()),
       'creationInfo': {'creators': ['Tool: CgPhone linux-sbom.py'], 'created': datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')},
       'comment': 'File inventory covers shipped payload. Debian packages are the build environment inventory, not a complete resolved runtime graph. Runtime dependencies are supplied by the distribution. License review remains pending.',
       'packages': packages, 'files': files, 'relationships': relationships}
pathlib.Path('dist/CgPhone-linux-sbom.spdx.json').write_text(json.dumps(doc, indent=2)+'\n')
